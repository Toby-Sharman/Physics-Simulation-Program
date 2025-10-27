# Physics Simulation Program

Simulate particle movement, objects, and some simple physical interactions. The focus is on low energy particle physics, 
i.e. non-relativistic or pure quantum mechanical (interactions and other physical effects can be based in these but the 
focus is for a more general closer analogy to real life macroscopic interactions).

All particle and object attributes are stored as a `Quantity` struct data type that combines a value and a unit.
For operations involving quantities, **the unit of the left hand operand is conserved**.

There is no preservation of unit context, since the program tracks units in terms of their SI dimensions and 
factor/scale to base SI units (m, kg, s, A, K, mol, cd). The unit dimensions are stored as integers and an error will be
thrown if an operation would make a non integer exponent.

For easy construction of particles and objects there are particle and material databases for common particles and their 
relevant attributes. These databases are defined in a user readable format `.json` and are converted to a binary format 
with the `json_to_bin.cpp` tool. To read the JSON files `nlohmann_json` is used and can be auto installed if not already 
by commenting out a section of the `CMakeLists.txt` file.

---

## Physics

Movement has relativistic considerations.

Interaction probability from optical depth (or mean free path = 1/optical depth), via the optical depth = number density x cross-section x distance, P = 1 - 
e^(-optical depth)

### Natural unit usage

Do I still bother with any natural units?

- Speed of light (c)
- Elementary charge (e)
- Planck constant (ℏ)

### Physical constants
All physical constant are from [CODATA recommended values of the fundamental physical
constants: 2022](https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=958143)

---

## Particles

Construct **individual particles** or a **bunch as a source**

Can be auto created based on standard particle attributes defined in the particle database

Photon momenta stored as a Stoke's vector

---

## Objects

Objects are arranged in a tree structure, where descendants overwrite the defined space of the parent. Objects are 
defined based on the local space of the parent, so translations and rotations must be defined around the origin/centre 
of the parent. To test the correct layout use `root object->printHierarchy()` to view the entire layout. **There is no 
handling for if the layout of objects extends outside a parent or intersects with another object so care must be taken 
in objects lest there be unforeseen consequences**.

### Supported object types
- Box
- Sphere

### Construction

Construction is done by either `construct<object type>(...)` or `Object pointer->addChild<object type>(...)`. 
Construction returns pointers for minimal overhead and better lifetime management. For more intuitive construction there 
are tag wrappers for each attribute:
- `parent(...)` defaults no `nullptr`
- `name(...)` defaults to `"Unknown"`
- `position(...)` defaults to the origin/centre of parent object
- `rotation(...)` defaults to identity matrix
- `size(...)` default varies on object type, but should be point like
- `material(...)` defaults to `"Unknown"`
- `temperature(...)` defaults to room temperature `293 K`
- `numberDensity(...)` must be defined
- `relativePermeability(...)` must be defined

The order of the tags is irrelevant and if there is any issue with the arguments an error will be thrown describing it.
The object only requires the **number density** and **relative permeability** to be defined either directly or from the 
material database via the material name, but it is heavily recommended that size and other relevant attributes be 
defined too.

---

## Improvements to make

- Readd templated type for matrices for easier creation of rotation matrices and minor overhead improvement
  - Will require rework of asMatrix function in TransformationMatrix
- For transformations between objects add lazy computation
- Add renormalisation for matrices after operations to reduce numerical drift
- For computing displacement vector it will be the same across many particles so lazy computation could be good, have an 
updating table of values that are keyed by the velocity
- Bounding radius for quicker computations for contains function
- Review file opening for writing data
- Unit printing not be the dimension representation
- Multithreading
  - In particular for the stepping of multiple particles

---

## Documentation

At the top of each file is the name of the project, the file, author, creation date, a brief description of what is in
file, and legal/license information.

Verbose commenting in headers and in source files any relevant comments to describe algorithms that aren't immediately
clear.

Comments in headers will focus on describing what the code is for, how it is used, and its limitations. Where relevant
there may also be comments on algorithms/logic if the implemented logic is peculiar, or that the approach is notable
to how the code is used.

Classes will keep a general description of all of their methods and an example of usage. For methods in classes
there will be a brief description of what the method does but any important details will be kept in a class wide comment
to avoid duplication of comments for similar function (for example addition and subtraction overloads having the
condition that the units have the same dimensions). Any private class methods will not be described in the class header
as to be more clear it is not a callable option. For virtual functions they will be mentioned, but no expansive 
documentation will be described in the parent class as to lend to variant logic in derived classes.

---

## License

This project is licensed under a Non-Commercial License.

You may use, modify, and share this software for non-commercial purposes only,
including academic research, personal projects, or teaching.

Commercial use, including selling or incorporating it into commercial products,
is strictly prohibited without prior written permission.

Proper attribution must be given to the original author. If using this software in research,
please acknowledge the author and/or cite the repository:

https://github.com/Toby-Sharman/Physics-Simulation-Program

See the full [LICENSE](LICENSE) file for complete terms.