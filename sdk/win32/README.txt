NOTES ON USING OFFSCREENGECKO SDK

The contained binary of OffscreenGecko was build against the XULrunner
1.9b2pre nightly from 2007-12-16. You can get binaries and SDKs from
http://ftp.mozilla.org/pub/mozilla.org/xulrunner/nightly/2007/12/2007-12-16-10-trunk/.
It may or may not work with later nightlies. 
You will either have to give the directory with the XULrunner binaries as
an additional path to search for XULrunner to OffscreenGecko (see API docs)
or set the GRE_HOME variable to that directory.

The embedding application must regularly pump messages to all Windows. (This
is usually the case.)
