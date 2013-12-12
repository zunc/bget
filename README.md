bget
====

Multi-thread downloader client<br>

<b>usage:</b><br>
 -t,	 --thread=NUMBER	thread count (default = 5)<br>
 -o,	 --output=FILE		destination of output file<br>
 -h,	 --help			show help<br>

<b>Example:</b><br>
>khoai@khoai-pc$ ./bgeter http://wallpapers.wallbase.cc/rozne/wallpaper-2461463.jpg<br>
>--- File information<br>
>	Name: wallpaper-2461463.jpg<br>
>	Temp files: .btmp/wallpaper-2461463.jpg.partN<br>
>	Size: 1279667<br>
>	Type: image/jpeg<br>
><br>
> -> Server support resume/multithread download<br>
>Downloaded: .btmp/wallpaper-2461463.jpg.part3<br>
>Downloaded: .btmp/wallpaper-2461463.jpg.part1<br>
>Downloaded: .btmp/wallpaper-2461463.jpg.part2<br>
>Downloaded: .btmp/wallpaper-2461463.jpg.part4<br>
>Downloaded: .btmp/wallpaper-2461463.jpg.part0<br>
> Download success, goodbye :*
