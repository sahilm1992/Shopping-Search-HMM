1. install "python 3.4" in "C Drive", Make sure "Python34" Folder is created under "C Drive"
	(Note that Installation File can be found in installation_speech_project directory)

2. Add "C:/Python34" to "PATH" Environment Variable

3. Install WAMP Server ( installation is present in installation_speech_project directory)
	If wamp symbol is not getting green in notification tray in windows task bar then Follow these steps:
	Disable skype,MsDepSVC from services.msc, since it blocks Port 80)
	If still wamp symbol is not green then click on wamp icon in notification tray-> apache ->services ->test port 80. 
	Disable the service which is blocking port 80 from services.msc
	Now, Restart WAMP Server.

4.	open ...wamp\bin\apache\apacheXXXX\conf\httpd.conf,
	->search & replace 
		Options Indexes FollowSymLinks 
		with 
		Options Indexes FollowSymLinks ExecCGI	
	-> search & replace 
		#AddHandler cgi-script .cgi 
		with (...removing # ) 
		AddHandler cgi-script .cgi 
		AddHandler cgi-script .py

	-> Find the line: DirectoryIndex index.php index.php3 index.html index.htm 
	and add in the end of them:  index.cgi index.py 
	Now, Restart WAMP Server.

5. copy the files in present in "files_to_be_copied" Directory to c:\wamp\www\

6. open browser and open http://localhost/Speech_Process/Debug/web.html

7. For testing single item one at a time :
	Click on Search items and speak word .( syrup, jeans , watch , phone or battery)
	
   For testing continously , click on " Click for continous recordings " , speak when button is labelled speak . 

8. For training new speaker click on Train for new user or open ( http://localhost/Speech_Process/Debug/addNew.html ).
	Record samples for new user for words ( syrup, jeans , watch , phone and battery)

9. For training new speaker using recorded files open http://localhost/Speech_Process/Debug/addNewFileUpload.html.
	suppply 30 files labelled as (watch1.txt to watch30.txt ) each for watch , syrup , jeans , phone and battery .
	

	
Prepared by 
Sahil Manchanda 154101019, (Mob:9654040804)
Saurabh khatavkar 154101061, (Mob:8097114121)