# What are `.sshg` files?

`.sshg` files are a custom json like file for describing groups of individual shader files. The name stands for **Siren
Shader Group**. It requires that all referenced shader files be in the same directory. It is currrently a limited file
format with the following general form:

```json
{
  "name": "Shader Group Name",
  "stages": {
    "vertex": "vertex.vs",
    "fragment": "frag.fs"
  }
}
```

A name, a vertex and a fragment shader are the minimum requirements for a valid `.sshg` file.