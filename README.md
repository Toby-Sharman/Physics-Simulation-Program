# Physics Simulation Program

Simulate particle movement, objects, and some simple physical interactions.

All particle and object attributes are stored as a `Quantity` struct data type that combines a value and a unit.
For operations involving quantities, **the unit of the left hand operand is conserved**.

There is no preservation of unit context, since the program tracks units in terms of their SI dimensions and 
factor/scale to base SI units (m, kg, s, A, K, mol, cd). The unit dimensions are stored as integers and an error will be
thrown if an operation would make a non integer exponent.

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

Can be auto created based on standard particle attributes defined in `particle_database.json`

Photon momenta stored as a Stoke's vector

---

## Objects

### Supported types 
- Box

---

## Improvements to make

- For transformations between objects add lazy computation
- Bounding radius for quicker computations for contains function

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
as to be more clear it is not a callable option.

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