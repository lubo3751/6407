import os
from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import gettempdir

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = gettempdir()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.route("/")
@login_required
def index():
    
    # shorcut to user's id
    id = session["user_id"]
    
    # get actual portfolio stock prices + users data
    stocks = db.execute("SELECT * FROM :id GROUP BY Symbol", id=str(id))
    for stock in stocks:
        stock['Price'] = update_price(stock['Symbol'])
        stock['Total'] = update_price(stock['Symbol'])*stock['Shares']
    user = db.execute("SELECT cash FROM users WHERE id = :id", id=id)
    
    # make it usd format
    total = 0
    for stock in stocks:
        total += stock['Total']
        stock['Price'] = usd(stock['Price'])
        stock['Total'] = usd(stock['Total'])
    total += user[0]['cash']
    user[0]['cash'] = usd(user[0]['cash'])
    total = usd(total)
    
    # print portfolio
    return render_template("index.html", stocks=stocks, user=user, total=total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    
    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        
        id = session["user_id"]
        
        # ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("MISSING SYMBOL")
            
        # ensure shares was submitted
        if not request.form.get("shares"):
            return apology("MISSING SHARES") 
        
        try:
            shares = int(request.form.get("shares"))
        except:
            return apology("INVALID SHARES")
        
        if shares < 0:
            return apology("INVALID SHARES")
        
        # get actual price
        quote = lookup(request.form.get("symbol"))
        
        # ensure symbol valid
        if not quote:
            return apology("INVALID SYMBOL")
        
        # ensure enough money
        cash = db.execute("SELECT cash FROM users WHERE id = :id", id=id)
        if float(cash[0]["cash"]) < (shares * quote["price"]):
            return apology("CAN'T AFFORD")
        
        # chech if stock already in portfolio
        rows = db.execute("SELECT * FROM :id WHERE Symbol = :symbol", id=str(id), symbol=quote['symbol'])
        
        # add shares of bought stock to user's portfolio
        if not rows:
            db.execute("INSERT INTO :id(Symbol, Name, Shares, Price, Total) VALUES(:Symbol,:Name, :Shares, :Price, :Total)", id=str(id), Symbol=quote['symbol'], Name=quote['name'], Shares=shares, Price=float(quote['price']), Total=float(shares*quote['price']))
        else:
            db.execute("UPDATE :id SET 'Shares'=:Shares, 'Price'=:Price, 'Total'=:Total WHERE Symbol=:symbol", id=str(id), symbol=quote['symbol'], Shares=shares+rows[0]['Shares'], Price=quote['price'], Total=((shares+rows[0]['Shares'])*quote['price']))
        
        # update user's cash
        db.execute("UPDATE 'users' SET 'cash'=:cash WHERE id=:id", cash=cash[0]["cash"]-(shares*quote["price"]), id=id)
        
        # create history table if not created
        db.execute("CREATE TABLE IF NOT EXISTS 'history' ('Symbol' TEXT NOT NULL, 'Shares' INTEGER, 'Price' REAL NOT NULL, 'Transacted' DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 'id' INTEGER)")
        
        # insert transaction to history table
        db.execute("INSERT INTO 'history'(Symbol,Shares,Price,id) VALUES(:Symbol,:Shares,:Price,:id)", Symbol=quote['symbol'], Shares=shares, Price=quote['price'], id=str(id))
        
        # print message
        flash('Bought!')
        
        # redirect user to home page
        return redirect(url_for("index"))
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    
    # request portfolio data
    id = session["user_id"]
    stocks = db.execute("SELECT * FROM history WHERE id = :id", id=str(id))
    
    # make it usd format
    for stock in stocks:
        stock['Price'] = usd(stock['Price'])
    
    # print portfolio data
    return render_template("history.html", stocks=stocks)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        
        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))
    
    
@app.route("/password", methods=["GET", "POST"])
def password():
    """Change password."""

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # keep id in id
        id = session["user_id"]
        
        # ensure old password was submitted
        old = request.form.get("old_password")
        if not old:
            return apology("MISSING OLD PASSWORD")
        
            
        # ensure new password was submitted
        new = request.form.get("new_password")
        if not new:
            return apology("MISSING NEW PASSWORD")

        # ensure new password differs old one
        if new == old:
            return apology("NEW PASSWORD MATCH OLD ONE")
            
        # ensure new passwords match
        if new != request.form.get("confirmation"):
            return apology("NEW PASSWORDS DON'T MATCH")
        
        # finally change password
        else:
            db.execute("UPDATE users SET hash=:hash WHERE id=:id", hash=pwd_context.encrypt(new), id=id)
            
        # print message
        flash('Password changed!')
        
        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("password.html")    


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    
    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        
        # ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("MISSING SYMBOL")
        
        # search quote via lookup()
        quote = lookup(request.form.get("symbol"))
        
        if not quote:
            return apology("INVALID SYMBOL")
            
        # make quote a usd format
        quote['price'] = usd(quote['price'])
        
        # print quote
        return render_template("quoted.html", quote=quote)
        
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""
    
    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        
        # ensure username was submitted
        if not request.form.get("username"):
            return apology("MISSING USERNAME")
            
        # ensure password was submitted
        if not request.form.get("password"):
            return apology("MISSING PASSWORD")

        # ensure username exists and password is correct
        if request.form.get("password") != request.form.get("confirmation"):
            return apology("PASSWORDS DON'T MATCH")
        
        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        
        # ensure username isn't already in database
        if len(rows) != 0:
            return apology("USERNAME TAKEN")
        
        # register new user and insert him to database
        db.execute("INSERT INTO users(username,hash) VALUES(:username,:hash)", username=request.form.get("username"),  hash=pwd_context.encrypt(request.form.get("password")))
        
        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        
        # remember which user has logged in
        session["user_id"] = rows[0]["id"]
        
        # create user's portfolio if not created
        db.execute("CREATE TABLE :id ('Symbol' TEXT PRIMARY KEY NOT NULL, 'Name' TEXT NOT NULL, 'Shares' INTEGER, 'Price' REAL NOT NULL, 'Total' REAL)",  id=str(session["user_id"]))

        # print message
        flash('Registered!')
        
        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    
    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        
        id = session["user_id"]
        
        # ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("MISSING SYMBOL")
        
        symbol = request.form.get("symbol").upper()
            
        # ensure shares was submitted
        if not request.form.get("shares"):
            return apology("MISSING SHARES") 
        
        try:
            shares = int(request.form.get("shares"))
        except:
            return apology("INVALID SHARES")
        
        if shares < 0:
            return apology("INVALID SHARES")
            
        # get actual price
        quote = lookup(symbol)
        
        # ensure symbol valid
        if not quote:
            return apology("INVALID SYMBOL")
        
        # ensure enough shares in portfolio
        owned = db.execute("SELECT * FROM :id WHERE Symbol = :symbol", id=str(id), symbol=symbol)
        if shares > owned[0]['Shares']:
            return apology("TOO MANY SHARES")
        
        # subtract shares from portfolio(id) table
        elif shares == owned[0]['Shares']:
            db.execute("DELETE FROM :id WHERE Symbol = :symbol", id=str(id), symbol=symbol)
        else:
            db.execute("UPDATE :id SET 'Shares'=:Shares, 'Price'=:Price, 'Total'=:Total WHERE Symbol=:symbol", id=str(id), symbol=symbol, Shares=owned[0]['Shares']-shares, Price=quote['price'], Total=((owned[0]['Shares']-shares)*quote['price']))
        
        # update user's cash
        cash = db.execute("SELECT cash FROM users WHERE id = :id", id=id)
        db.execute("UPDATE 'users' SET 'cash'=:cash WHERE id=:id", cash=cash[0]["cash"]+(shares*quote["price"]), id=id)
        
        # insert transaction into history table
        db.execute("INSERT INTO 'history'(Symbol,Shares,Price,id) VALUES(:Symbol,:Shares,:Price,:id)", Symbol=quote['symbol'], Shares=-shares, Price=quote['price'], id=str(id))
        
        # print message
        flash('Sold!')
        
        # redirect user to home page
        return redirect(url_for("index"))
    else:
        return render_template("sell.html")
    
    # return apology("TODO")
