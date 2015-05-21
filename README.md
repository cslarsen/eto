Eto by Christian Stigen Larsen

    ./eto
    objects: 0
    alloc 0x7faf29c04be0 #<integer: 2>
    alloc 0x7faf29c04c50 #<integer: 1>
    alloc 0x7faf29c04cb0 #<cons: car=0x7faf29c04be0 cdr=0x0>
    alloc 0x7faf29d00030 #<integer: 0>
    alloc 0x7faf29d00060 #<cons: car=0x7faf29c04c50 cdr=0x7faf29c04cb0>
    alloc 0x7faf29d000f0 #<cons: car=0x7faf29d00030 cdr=0x7faf29d00060>
    ** root: 0 1 2
    objects: 6
    mark: looking at 0x7faf29d00060
    mark: in-use 0x7faf29d00060
    mark: looking at 0x7faf29c04c50
    mark: in-use 0x7faf29c04c50
    mark: looking at 0x7faf29c04cb0
    mark: in-use 0x7faf29c04cb0
    mark: looking at 0x7faf29c04be0
    mark: in-use 0x7faf29c04be0
    mark: looking at 0x0
    sweep: unreachable 0x7faf29d00030
    deleting 0x7faf29d00030 `#<integer: 0>'
    sweep: unreachable 0x7faf29d000f0
    deleting 0x7faf29d000f0 `#<cons: car=0x7faf29d00030 cdr=0x7faf29d00060>'
    collected 2 objects
    ** root: 1 2
    objects: 4
    exiting ...
    deleting 0x7faf29c04be0 `#<integer: 2>'
    deleting 0x7faf29c04cb0 `#<cons: car=0x7faf29c04be0 cdr=0x0>'
    deleting 0x7faf29c04c50 `#<integer: 1>'
