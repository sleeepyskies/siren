Checklist kinda for me:

should include #SOBJ_IMPLEMENTATION pattern
find a better way to iterate over streams without setting the failbit when reaching EOF LOL
=> this also means we have no error checking for correct format of f lines

normalize normals?
https://stackoverflow.com/questions/29515405/how-many-vaos-and-vbos

https://docs.fileformat.com/3d/obj/
https://paulbourke.net/dataformats/obj/
https://paulbourke.net/dataformats/mtl/

Supported .obj features:

- Vertices
- Normals
- Texture Coordinates
- Comments
- Faces
    - Only vertices
    - Vertices and UV
    - Vertices, UV and normals
    - Vertices and normals
    - Vertex counts of 3
- Check at least v line is present
- Relative and Absolute Indices (no checking for invalid negative indices yet)

Unsupported .obj features:

- Faces
    - Vertex counts greater than 3
    - Groups sharing faces
- UVs of vec3
- Groups
- Names Groups
- Line Elements
- Reference Materials
- Smooth Shading
- Named Objects
- Materials
- Colors
- vec4 positional coordinates
