The Faptime Shortener
=====================

### Author ###

Kyle Wanamaker
[@kylewpppd](http://twitter.com/kylewpppd)

### Purpose ###

I wanted to build a small project in C using Redis. Seeing that every
web tutorial ever builds a twitter clone or a url shortner, I figured why not
build a shortener made with cool stuff. I wanted to learn more about POSIX
sockets, so I created Faptime. As of this writing, I am using FastCGI rather
than the sockets API because I wanted to get something out the door, then
"improve" Faptime by using the sockets API.


### Requirements & Dependencies ###

Faptime is written in C and has a few external dependencies outside of the C
Standard Library.

- **[Hiredis](http://github.com/redis/hiredis):** The official C Redis library.
- **[libcurl](https://github.com/bagder/curl):** Used for URL encoding and
  decoding. I thought it was a little too dangerous to attempt this myself.
- **[FastCGI](http://www.fastcgi.com/):** This may be removed in the future if
the project moves to using sockets.


### Why's It Called "Faptime.biz"? ###

Good question. A lot of guys I work with have personal url shorteners. They drop
their shortened urls in chats and email. I'm a light-hearted guy and wanted to
have a shortener that would make people chuckle.

Sure I could use CloudApp or some other service, but why? I'm an engineer, and I
like C and Redis. So, I figured I'd roll my own. Not only should mine be faster,
it should also have a better name.


### Contributing ###

I'm happy to have anyone help out with Faptime. Right now it isn't even close to
being production ready.

### License ###

Faptime copyright (c) 2013 Kyle Wanamaker and is licensed under the
**[GNU GPLv3 license](http://www.gnu.org/licenses/gpl.html)**. If you would like
to use Faptime under a different license, please contact me.

Software in linked libraries are covered by their own licenses. They are not
distributed with Faptime.