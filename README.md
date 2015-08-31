The code in this repository is designed to deal with non-rectangular data, such as time series collected as part of a web-scraping exercise. Below you see a sample of web-scraping that recorded the ranks of the top-25 Reddit posts every hour. For each post, the rank and time the rank was recorded were collected until the post fell out of the top 25. This resulted in a .tsv file, with example lines below, where each row represents a single post and its trajectory through the front page of Reddit.  
````
comp	id	subreddit	created	ranks	recorded_at	rank_length
0	3ckf7b	pics	1436373189.0	[1, 1, 3, 5]	[1436392502, 1436396101, 1436399701, 1436403301]	4.0
0	3csv79	UpliftingNews	1436538581.0	[16, 24]	[1436558101, 1436594101]	2.0
0	3ccx4y	gifs	1436223351.0	[6, 7, 7, 10]	[1436259301, 1436262901, 1436266501, 1436273701]	4.0
0	3bldf2	todayilearned	1435636909.0	[4, 3]	[1435665301, 1435668901]	2.0
0	3acrl2	pics	1434677487.0	[0, 0, 0, 4]	[1434686101, 1434689701, 1434693301, 1434714901]	4.0
0	3cosrl	space	1436457300.0	[22, 16, 15, 14, 15, 17, 15, 18, 18]	[1436489702, 1436493301, 1436496901, 1436500501, 1436504101, 1436507701, 1436511301, 1436518501, 1436522101]	9.0
0	3d2m5l	pics	1436748860.0	[6]	[1436781302]	1.0
0	3b5ll4	nottheonion	1435291130.0	[14, 14, 17]	[1435326901, 1435330501, 1435334101]	3.0
0	3a7l67	Showerthoughts	1434575878.0	[16, 13]	[1434617702, 1434628502]	2.0
````

The goal of the code is to convert information like the above into plots that look like what we see below. 

![ "data image" ](https://github.com/sunnysideprodcorp/PlottingNonRectangularData/blob/master/compare1.jpg "Data plot")

These two-dimensional histogram plots are an instructive way to examine non-rectangular data because these plots do not impose any assumptions about the shape or length of a time series such as the ones we collected when scraping Reddit's front page.  We can get a sense of how different subreddits perform by watching how individual posts perform when they first enter the top 25 posts, and how long they stay there. For example, in the figure below we see that videos tend to shoot to the top of the rankings but not last a long time. In contrast TwoXChromosomes tends not to rank as highly as videos but does tend to stay around a bit longer in the top 25.

The code includes have different alignment options to learn different things about the data. In the above plot, where all time series are aligned together at their first point, we learn about how a threads in a certain subreddit tend to over time. But we might also ask how different subreddits behave at different times of the day. We can see that information below, in an example plot. In particular, we see that personalfinance is largely popular in the mid to late evening hours, whereas nottheonion is popular almost every hour of the day. nottheonion is overall also more popular, achieving higher ranks and a higher overall count compared to personalfinance.

![ "data image" ](https://github.com/sunnysideprodcorp/PlottingNonRectangularData/blob/master/compare1.jpg "Data plot")

Anyone using or modifying the C++ code should be aware of a few particulars:

1. There is no error checking in the Hist2D class. You must be sure that the max and min values you use to initialize a Hist2D will cover all your data. If not, you will have a run-time error that could waste a lot of time. The decision was made to avoid error checking in favor of speed.
2. In NumericVector, summary statistics are all computed as doubles. If you are using especially troublesome arithmetic types, such as long ints, these summary statistics may not cast correctly. You must check these values or insert error checking and appropriate casting. Again the decision was made to keep the code light weight and speedy.
