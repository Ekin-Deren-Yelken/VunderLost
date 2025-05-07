import sys
from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer

analyzer = SentimentIntensityAnalyzer()
text = " ".join(sys.argv[1:])
score = analyzer.polarity_scores(text)

# Custom labels
if score['compound'] >= 0.7:
    print("positive")
elif score['compound'] <= -0.3:
    print("negative")
else:
    print("neutral")
