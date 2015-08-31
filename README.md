
The C++ processing files were designed to extract lists of numbers where the length of each list (stored in rows of a textfile) is unknown and where the list can be composed of any kind of arithmetic type.

The files create a NumericVector class that stored numeric data in (x,y) pairs where y represents the value of interest and x can be a corresponding secondary value or a displacement measurement to order the y-pairs.

In the particular demo shown, data is extracted from data.tsv, the result of a webscraping exercise that recorded the top 25 Reddit posts every hour. data.tsv contains this data amalgamated so that each row of the file contains the information for one posting. For each posting, the corresponding subreddit was recording as well as the timestamp for the creation of the thread and two lists, one indicating the rank and one indicating the corresponding timestamp for that rank. In this way individual postings can be examined in terms of their behavior over time. Some additional data is available in data.tsv but not used here.

NumericVector provides a handy class for matching the rank of a posting against the time that particular rank was recorded. VectorOfNumericVectors creates a vector of these NumericVectors, a handy container class for grouping NumericVectors and also for taking statistics of their ensemble rather than their individual trajectories. Finally Hist2D is effectively a Boost::Multi_Array wrapper class that uses the Multi_Array as a two-dimensional histogram. Y-values can be plotted relative to their indices or relative to their corresponding X-values. 

You can see the kinds of plots that can be made by looking over the pdf files in the data folder. Some examples are below.

We can get a sense of how different subreddits perform by watching how individual posts perform when they first enter the top 25 posts, and how long they stay there. For example, in the figure below we see that videos tend to shoot to the top of the rankings but not last a long time. In contrast TwoXChromosomes tends not to rank as highly as videos but does tend to stay around a bit longer in the top 25.

![ "data image" ](https://github.com/sunnysideprodcorp/PlottingNonRectangularData/blob/master/compare1.jpg "Data plot")

We have different alignments to learn different things about the data. In addition to seeing how a subreddit does over time, we can also see how time of day affects subreddits. In the image below, we can see that some subreddits are quite time specific. In particular, we see that personalfinance is largely popular in the mid to late evening hours, whereas nottheonion is popular almost every hour of the day. nottheonion is overall also more popular, achieving higher ranks and a higher overall count compared to personalfinance.

![ "data image" ](https://github.com/sunnysideprodcorp/PlottingNonRectangularData/blob/master/compare1.jpg "Data plot")