Make sure opencv is installed.

- **For assignment check**:

```bash
bash test.sh
```

- To run the executable seperataly
```bash
cd path_to_executable

# after compilation

./Assignment2 ${Mesh type} ${Angle} ${Output path of images}
``` 

The available mesh type includes:
- Circle
- Square
- Triangle
- Cube
- Customized obj file. In this case, user should input the path to obj file into `${Mesh type}`. For example:
    ```
    ./Assignment2 ${OBJ_PATH}/african_head.obj # This will call a window render african_head.obj in wireframe mode.
    ```

