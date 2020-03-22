# SE-315

## Lab 1 Docs

### Environment

Provided VMware virtual machine was used, based on VMware Fusion Pro (`v11`).

Files were tracked under `git` version control system.

### Exercises

#### Exercise 1

Differences between x86-64...

##### Registers

* More Registers.
	* `ARMv8`, as a *CISC* ISA, has more than 30 general purpose registers, while `x86-64` has only 16 visible registers (and only 8 of them can be used for general purposes).
* Isolated Registers for Exception Levels.
	* There are 4 different privilege levels in `ARMv8` Architecture. Hardware registers are provided separately to ensure the isolation.
	* However, `x86-64` has no such hardware isolation design.
* Saved Program Status Registers Manipulating Methods.
	* Some critical runtime states are saved as grouped bits in a special register called SPSR. Using `LDR` or `STR` can directly manipulate that register.
	* `x86-64` has a counterpart called `EFLAGS`, however it can't be simply read or written by general instructions, but with special instructions like `LAHF` and `SAHF`.

##### Privilege Levels

* The Greater, the Higher.
	* In `ARMv8`, there are 4 exception levels. The greater the level number is, the higher privilege it could obtain.
		* Generally, the OS kernel **doesn't** run in the highest privilege level `EL3`, but in `EL1`, just one level higher than user-modal applications.
		* `EL2` and `EL3` is reserved by hardware monitor codes which is strictly verified flawlessly.
	* However, the *Protection Ring* System as the counterpart in `x86-64` works differently.  The lesser the level number is, the higher privilege it could obtain.
		* Generally, the OS kernel runs in the highest privilege level *Ring 0*.
		* There's no higher-privileged system working beyond the OS kernel. So any bugs lies in OS kernel could be fatal.

##### Instructions

* Instruction Lengths: Static or Dynamic.
	* `ARMv8`'s instructions have all the same length, which is 32 bits, or 4 Bytes.
		* That means *PC* (Program Counter)'s will always increase itself by 4 (Bytes) after each instruction fetching.
	* However, `x86-64` 's instruction length isn't static. That could reduce the binary code size but complicate the integrated-circuit implementation.
* Different Parallel Computing Implementation.
	* In `ARMv8`, spacial parallel computation is implemented via *aSIMD* technique.
	* However in `x86-64`, it was implemented by *SSE* technique.

* Conditional Instructions.
	* In `ARMv8`, there are very few conditional instructions left.
		* The only one is the `B.cond` instruction, which judges the `cond` condition and decide if it should perform a branch jump.
	* However in `x86-64`, there are much more conditional instructions like `cmov` (conditional moving).
* Updating Condition Bits.
	* In `x86-64`, all arithmetic instructions will automatically update the condition bits.
	* However in `ARMv8`, those normal arithmetic instructions won't update those bits.
		* Only those with `S` prefix will set the condition bits.

#### Exercise 2

After executing `make gdb`, the first label i can see is `arm64_elX_to_el1`, which helps switching the exception level from higher `ELx` to OS kernel's `EL1`.

After that function exits, the CPU runs in `EL1` privilege level and the very first function called in the kernel is `_start`.

```
(gdb) where
#0  0x0000000000080000 in _start ()
```

Its address appears to be `0x0000000000080000`.

By analyzing the kernel image `./build/kernel.img`, here's what i got:

```
Symbol table '.symtab' contains 119 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000080000     0 SECTION LOCAL  DEFAULT    1 
     2: ffffff00000cc000     0 SECTION LOCAL  DEFAULT    2 
	... ...
    91: 0000000000080000    48 FUNC    GLOBAL DEFAULT    1 _start
	... ...
   117: 0000000000086000  4096 OBJECT  GLOBAL DEFAULT    1 _boot_pgd_down
   118: 0000000000087210   160 FUNC    GLOBAL DEFAULT    1 el1_mmu_activate

```

Symbol #91 is the first function mentioned above.

> Document Typo:
>
> Option `-S` (capital) or `--sections` actually prints the section headers rather than the symbol table.
>
> Option `-s` (non-capital) or `--symbols` should print the symbol table well.

