
make simple things simple

Safe, C style constructions:

* memory
	- The concept of memory blocks [begin,end)
	- Cursors
	- Fixed sized strings (stack based)
	- Fixed sizes binary blocks (stack based)
	- Growable buffers (heap allocated fallback)
	- virtual interface for hiding expansion

* formatting
	- Automatic conversion of int/float to string
	- Format directed conversion

* io/file
	- stdio/stdout/stderr
	- automatic line endings for simple messages
	- differentiation between text and binary
	- differentiation between buffered and raw
	- exception agnostic

And complex things, not too bad.

* network
	- basic synchronous operations
	- asynchronous ports
	- watch handlers
	- processing chains

* pipes
* compression
* encryption
* shared memory/mmap

