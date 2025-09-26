# Physics Simulation Program

Simulate particle movement, objects, and some simple physical interactions.

All particle and object attributes are stored as a `Quantity` struct data type that combines a value and a unit.
For operations involving quantities, **the unit of the left hand operand is conserved**.

---

## Physics

Movement has relativistic considerations.

### Natural unit usage
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

---

## Objects

### Supported types 
- Box

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