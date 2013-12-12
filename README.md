bget
====

Multi-thread downloader client

usage:
 -t,	 --thread=NUMBER	thread count (default = 5)
 -o,	 --output=FILE		destination of output file
 -h,	 --help			 	show help

Example:
khoai@khoai-pc$ ./bgeter http://wallpapers.wallbase.cc/rozne/wallpaper-2461463.jpg
--- File information
	Name: wallpaper-2461463.jpg
	Temp files: .btmp/wallpaper-2461463.jpg.partN
	Size: 1279667
	Type: image/jpeg

 -> Server support resume/multithread download
Downloaded: .btmp/wallpaper-2461463.jpg.part3
Downloaded: .btmp/wallpaper-2461463.jpg.part1
Downloaded: .btmp/wallpaper-2461463.jpg.part2
Downloaded: .btmp/wallpaper-2461463.jpg.part4
Downloaded: .btmp/wallpaper-2461463.jpg.part0
> Download success, goodbye :*