# Physics Simulation Program

The current main goal is to simulate OPMs (Optically pumped magnetometers) and as such interactions included may well be
limited to what is relevant to them.

Simulate particle movement, objects, and some simple physical interactions. The focus is on low energy particle physics, 
i.e. non-relativistic or pure quantum mechanical (interactions and other physical effects can be based in these but the 
focus is for a more general closer analogy to real life macroscopic interactions).

There is no preservation of unit context, since the program tracks units in terms of their SI dimensions and 
factor/scale to base SI units (m, kg, s, A, K, mol, cd). The unit dimensions are stored as integers and an error will be
thrown if an operation would make a non integer exponent.

For easy construction of particles and objects there are particle and material databases for common particles and their 
relevant attributes. These databases are defined in a user readable format `.json` and are converted to a binary format 
with the `json_to_bin.cpp` tool. To read the JSON files `nlohmann_json` is used and can be auto installed if not already 
by commenting out a section of the `CMakeLists.txt` file.

For reasonable computation time multithreading and Monte Carlo techniques are employed and as such provided is a 
deterministic random generation method is provided via `random_manager`. This is thread safe and allows for different 
streams different purposes, e.g. separate stream for interactions as thermal velocity sampling.

---

## Physics

Discrete interactions are handled based on calculating interaction lengths and from them then determining the 
interaction to occur based on. Currently, the interaction selector is not properly implemented due a lack of interaction
types not requiring it.

### Physical constants

All physical constants are from [CODATA recommended values of the fundamental physical constants: 2022](https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=958143).

Current particle and material database data are nonsensical and are not to be taken as accurate values.

---

## Particles

It is recommended to construct particles via a source object and not individually so they can be properly added to the 
particle manager. Current particle source allows for variation in some attributes like momentum and position but this is
not yet well implemented so should be treated with great care to ensure constructed particles are realistic.

### Supported particle types

- Photon
- Atom (has support for hyperfine levels but needs testing)
- Generic (not recommended for use)

---

## Objects

Objects are arranged in a tree structure, where descendants overwrite the defined space of the parent. Objects are 
defined based on the local space of the parent, so translations and rotations must be defined around the origin/centre 
of the parent. To test the correct layout use `root object->printHierarchy()` to view the entire layout. **There is no 
handling for if the layout of objects extends outside a parent or intersects with another object so care must be taken 
in objects lest there be unforeseen consequences**.

For semantic reasons the highest level object is typically referred to as the world object and must be referred to the 
object manager else it will not be properly linked into the step loop.

### Supported object types

- Box
- Sphere

### Construction

Construction is done by either `construct<object type>(...)` or `Object pointer->addChild<object type>(...)`. Using 
construct requires a parent argument to be assigned and if addChild is called it requires a lack of parent argument. 
**Do not use construct sans a parent argument lest it not be picked up by the object manager and linked into
the stepping loop.** Currently, with the new object manager the world object should be created with the `createWorld` 
method of the object manager. Construction returns pointers for minimal overhead and better lifetime management. For 
more intuitive construction there are tag wrappers for each attribute:
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

## Simulation loop

There is a `stepAll(...)` function that will advance particles by either the given time or by the default value in 
`config/program_config.h`. Output/database locations and default filenames are configured in `config/path_config.h`. 
Particles will attempt to advance by the entire time step just undergoing continuous processes but the step may be 
limited by different discrete processes, such as interactions, decays, and boundary collisions. After a limiter is 
determined the particle will advance by the interaction length, undergoing the implemented continuous processes, and 
once finished complete that discrete interaction. Currently, after the step limiter limits the effective travel time it 
will be attempted to finish the rest of the step on the resultant stuff from the interaction, but this may well change 
with better handling of particles post interaction to aid in better handling of interactions that would produce multiple
particles. Particles that reach either a specified collector, where they will be logged, or outside the world object are 
removed from the simulation.

Boundary collisions has a different implementation to other discrete interactions since it is a guarantee to occur not a
likelihood.

The `stepAll(...)` function should be slotted in a loop to step until all particles are removed. Later this will be 
implemented as standard with some option to add a limit on either program run time or simulation time (_this is a rough 
plan and may well change_).

### Supported processes

- Continuous
  - Particle movement under momentum only (has relativistic considerations)
- Discrete
  - Boundary collisions
  - Photon absorption
  - Spontaneous emission

---

## Improvements to make

- Readd templated type for matrices for easier creation of rotation matrices and minor overhead improvement
  - Will require rework of asMatrix function in TransformationMatrix
- For transformations between objects add lazy computation
- Add renormalisation for matrices after operations to reduce numerical drift
- For computing displacement vector it will be the same across many particles so lazy computation could be good, have an 
updating table of values that are keyed by the velocity
- Bounding radius for quicker computations for contains function
- Unit printing not be the dimension representation
- Have a smaller database of values kept in memory that has the often used particles (maybe materials too but *much* 
less significant) so reading the entire database is not required (for instance for creating photons during spontaneous 
emission)
- Have Quantity operations that return dimensionless return a double not a Quantity

---

## Documentation

At the top of each file is the name of the project, the file, author, creation date, a brief description of what is in
file, and legal/licence information.

Main description of how stuff functions should be in the headers not in the source files except in the case where an 
object/function/other is limited to just the source file and it that case there may be description there for its 
functionality.

Classes will keep a general description of all of their methods and an example of usage. For methods in classes
there will be a brief description of what the method does but any important details will be kept in a class wide comment
to avoid duplication of comments for similar function (for example addition and subtraction overloads having the
condition that the units have the same dimensions). Any private class methods will not be described in the class header
as to be more clear it is not a callable option. For virtual functions they will be mentioned, but no expansive 
documentation will be described in the parent class as to lend to variant logic in derived classes.

---

## Licence

This project is licensed under a Non-Commercial Licence.

You may use, modify, and share this software for non-commercial purposes only, including academic research, personal 
projects, or teaching.

Commercial use, including selling or incorporating it into commercial products, is strictly prohibited without prior 
written permission.

Proper attribution must be given to the original author. If using this software in research, please acknowledge the 
author and/or cite the repository:

https://github.com/Toby-Sharman/Physics-Simulation-Program

See the full [LICENCE](LICENCE) file for complete terms.
