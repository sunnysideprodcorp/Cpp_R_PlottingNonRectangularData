#This script generates a pdf showing all 2d histogrames produced by the c++ code in the working directory
#use the to_get var to select a subset of the histograms (for example "_byX_"), a filename characteristic set in the c++ code

#############################
#variables to set by data set


#this sets upper density bound in histogram for 15 colors
#an additional catchall bin is appended up to 1
reasonable_upper_density_bound <- .2 
#number of colors to spread uniformly between 0 and reasonable_upper_density_bound
num_colors                     <- 15

#sets file names script will look for in directory
to_keep                        <- "_byX_"
file_extension                 <- "txt"

#sets graphing parameters for output pdf
max_num_files                  <- 50
graphs_per_page                <- 6

###############################


#getting list of files selected by to_keep
files <- list.files()
files <- files[substring(files, nchar(files)-2, nchar(files)) == file_extension]
files <- files[substring(files, 1, 4)!="subr"]
files <- files[substring(files, 1, 4) !="anno"]
files <- files[grep(to_keep, files)]

#setting destination pdf
pdf_name <- paste0("ALL_REDDITS", to_keep, ".pdf")
pdf(pdf_name)

#this outer for loop runs once per page
for(i in 0:ceiling(max_num_files/graphs_per_page)){
  
  #setting graphical parameters and desired graphs for a given page
  par(mfrow=c(graphs_per_page/2,2))
  desired_indices <- c((1+i*graphs_per_page):(graphs_per_page+i*graphs_per_page))
  
  #this inner for loop runs for each histogram generated on each page
  for(filename in files[desired_indices]){
    
    #content files contain first 2 lines as parameter dataframe
    #rest of file is 2d histogram, xvalues are columns and y values are rows
    all_content    <- readLines(filename)
    first_two      <- all_content[c(1, 2)]
    skip_first_two <- all_content[c(-1, -2)]
    mydata         <- read.csv(textConnection(skip_first_two), header=FALSE)
    parameters     <- read.csv(textConnection(first_two))
    
    #normalize the 2d histogram to sum to 1
    norm_factor  <-  sum(colSums(Filter(is.numeric, mydata)))
    data_mat     <- data.matrix(mydata[ , 1:(ncol(mydata))])
    data_rotated <- apply(data_mat, 2, rev)
    data_rotated <- data_rotated/norm_factor
    
    #create image of 2d histogram, number of colors set to 15   
    image( c(0:(ncol(data_rotated))), c(0:(nrow(data_rotated))), t(data_rotated)
           , breaks = c(seq(0, reasonable_upper_density_bound, length.out = num_colors), 1),
           col=colorRampPalette(c(rgb(0,0,1,0), rgb(0,0,1,1)), alpha = TRUE)(num_colors), axes=FALSE,
           xlab="time of day (EST)", ylab = "rank within top 25")
    
    #title is set by file name and sum of unnormed histogram
    mainTitle <- substring(filename, 1, (nchar(filename) - (nchar(file_extension) + 1) - nchar(to_keep)))
    mainTitle <- paste(mainTitle, " - ", norm_factor, " samples")
    title(main=mainTitle)
    
    #axes are set according to parameter dataframe at top of data file
    axis(1, c(0:(ncol(data_rotated))), round(seq(from = parameters$xmin[1], to= parameters$xmax[1] , length.out = (parameters$xbins[1] + 1)), digits=0))   
    axis(2, c(0:(nrow(data_rotated))), round(seq(from = parameters$ymax[1] + 1, to=parameters$ymin[1] +1, length.out = (parameters$ybins[1] + 1)), digits=0))		
  }
}
dev.off()



