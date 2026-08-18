# Pasos ros2 andino



## Connexion mediante ssh

Para eso debe haber instalado un programa, ir a la seccion correspondiente

```bash
ssh andino@ubuntu.local
```

Si se conecta por el router tplink, ya posee ip fija

```bash
192.168.0.100
```



## Colocar RPI en modo ahorro de energia

Esto se hace para que el procesamiento no de un pico de corriente que haga que se apague

#### Modo ECO

```bash
echo powersave | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

#### Modo normal (alguno de los dos)

```bash
echo ondemand | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

echo schedutil | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

#### Verificar modo

```bash
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
```



## Ir al workspace 

```bash
cd ~/robot_ws
source install/setup.bash
```

## source para ros2 en pc

```bash
source /opt/ros/jazzy/setup.bash
```



## Funcionamiento de ROS2

#### Nodo general

```bash
ros2 launch andino_bringup andino_robot.launch.py
```

#### Nodo de navegacion

Debe estar cargado el mapa

```bash
ros2 launch andino_navigation bringup.launch.py map:=mapa_robotica.yaml
```

#### Guarda mapa luego de hacer slam

```bash
ros2 run nav2_map_server map_saver_cli -f mapa_robotica
```



#### Nodo para controlar por teclado

```bash
ros2 launch andino_bringup teleop_keyboard.launch.py
```







