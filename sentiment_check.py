from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer
import sys

text = " ".join(sys.argv[1:])
analyzer = SentimentIntensityAnalyzer()
score = analyzer.polarity_scores(text)['compound']

if score >= 0.4:
    print("agree")
elif score <= -0.4:
    print("disagree")
else:
    print("neutral")
