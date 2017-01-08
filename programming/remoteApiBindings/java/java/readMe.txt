Make sure you have following files in your directory, in order to run the simpleTest example:

1. folder "coppelia"
3. the appropriate remote API library: "remoteApiJava.dll" (Windows), "libremoteApi.dylib" (Mac) or "libremoteApi.so" (Linux)
4. simpleTest.java

You might also have to add the folder to the system path. In Linux for instance, you could call:

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`

before executing simpleTest.
