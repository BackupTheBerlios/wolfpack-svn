
import wolfpack

def onLogin( player ):
	socket = player.socket
	socket.sysmessage( "Welcome to %s" % ( wolfpack.serverversion() )  )
	socket.sysmessage( "Report Bugs: http://bugs.wpdev.org/" )
	player.hidden = 0
	player.update()
	socket.resendplayer()

def onLogout( player ):
	socket = player.socket
	player.removefromview()
	player.hidden = 1
	player.update()
