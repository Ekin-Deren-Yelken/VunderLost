import sys
from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer # type: ignore

analyzer = SentimentIntensityAnalyzer()
text = " ".join(sys.argv[1:])
score = analyzer.polarity_scores(text)

if score['compound'] > 0:
    print("friendly")
elif score['compound'] < 0:
    print("hostile")
else:
    print("neutral")