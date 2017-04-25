import nltk
import tokenize

class Analyzer():
    """Implements sentiment analysis."""

    # lists of positive / negative words
    pstv = []
    ngtv = []
    
    def __init__(self, positives, negatives):
        """Initialize Analyzer."""
        
        # load positive words to pstv
        with open(positives, encoding="utf-8") as pos:
            self.pstv.append(pos.read())
            self.pstv = self.pstv[0].split("\n\n", 1)
            self.pstv = self.pstv[1].split()
          
        # load negative words to ngtv
        with open(negatives, encoding="utf-8") as neg:
            self.ngtv.append(neg.read())
            self.ngtv = self.ngtv[0].split("\n\n", 1)
            self.ngtv = self.ngtv[1].split()

    def analyze(self, text):
        """Analyze text for sentiment, returning its score."""
            
        # tokenize text
        tokenizer = nltk.tokenize.TweetTokenizer()
        tokens = tokenizer.tokenize(text)
        
        # calculate score
        score = 0
        
        for token in tokens:
            if token.lower() in self.pstv:
                score += 1
            elif token.lower() in self.ngtv:
                score -= 1
                
        return score
        