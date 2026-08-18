# Bitácora — Raspberry Pi 5 + IMX219 + Ubuntu 24.04 + ROS 2 Jazzy

## Objetivo

Configurar una cámara Raspberry Pi IMX219 conectada por MIPI/CSI a una Raspberry Pi 5 con:

- Ubuntu 24.04 Noble 64-bit
- ROS 2 Jazzy
- Raspberry Pi 5
- Cámara IMX219
- libcamera
- `camera_ros`

La configuración final utiliza:

    IMX219
       ↓
    RP1 CSI / PiSP
       ↓
    libcamera 0.6
       ↓
    camera_ros
       ↓
    ROS 2 topics

No se utiliza `v4l2_camera` para esta cámara.

---

# 1. Comprobar la configuración de la cámara

Archivo:

    /boot/firmware/config.txt

La configuración que terminó funcionando tenía:

    camera_auto_detect=1
    display_auto_detect=1

No fue necesario agregar manualmente:

    dtoverlay=imx219

La cámara fue detectada automáticamente.

La sección relevante quedó:

    # Autoload overlays for any recognized cameras or displays
    camera_auto_detect=1
    display_auto_detect=1

IMPORTANTE:

No modificar `camera_auto_detect` a `0` ni agregar manualmente `dtoverlay=imx219,cam0`
si la detección automática ya funciona.

---

# 2. Comprobar que el kernel detecta la cámara

Ejecutar:

    sudo dmesg | grep -Ei "imx219|camera|csi|rp1|pisp"

La salida correcta debe contener algo equivalente a:

    found subdevice ... imx219@10
    Using sensor imx219 10-0010 for capture

y:

    Registered [rp1-cfe-csi2_ch0] ... as /dev/video0

En nuestro caso apareció:

    rp1-cfe 1f00110000.csi: found subdevice /axi/pcie@120000/rp1/i2c@88000/imx219@10
    
    rp1-cfe 1f00110000.csi: Using sensor imx219 10-0010 for capture
    
    rp1-cfe 1f00110000.csi: Registered [rp1-cfe-csi2_ch0] node id 0 successfully as /dev/video0

Esto confirmó que:

- el RP1 funciona
- el CSI funciona
- la IMX219 responde por I2C
- el kernel reconoce el sensor
- el dispositivo de captura fue registrado

Los mensajes:

    Fixed dependency cycle(s)

no impidieron el funcionamiento y no fueron el problema.

---

# 3. Comprobar permisos del usuario

La cámara utiliza dispositivos pertenecientes al grupo `video`.

Comprobar:

    groups

Si el usuario no pertenece a `video`:

    sudo usermod -a -G video $USER

Después cerrar sesión/reiniciar para que el cambio tenga efecto.

También se puede comprobar:

    ls -l /dev/v4l-subdev*

En nuestro caso:

    crw-rw---- 1 root video ...

por lo que los dispositivos estaban correctamente asociados al grupo `video`.

---

# 4. Problema inicial con libcamera

Inicialmente Ubuntu tenía:

    libcamera 0.2.0

Comprobación:

    apt policy libcamera-tools

Resultado inicial:

    Installed: 0.2.0-3fakesync1build6

y:

    dpkg -l | grep libcamera

mostraba:

    libcamera0.2
    libcamera-tools
    libcamera-ipa
    gstreamer1.0-libcamera

Esto no era suficiente para la configuración que necesitábamos.

La guía indicaba utilizar el PPA:

    ppa:marco-sonic/rasppios

---

# 5. Agregar el PPA de Marco Sonic

Ejecutar:

    sudo add-apt-repository ppa:marco-sonic/rasppios

Aceptar con ENTER.

Después:

    sudo apt update

Comprobar:

    apt policy libcamera-tools

Debe aparecer una versión del PPA similar a:

    0.6.0+rpt20251202-1ubuntu1~marco1

También:

    apt search libcamera | grep 0.

debe mostrar:

    libcamera0.6
    libcamera-tools
    libcamera-dev
    libcamera-ipa
    libcamera-v4l2
    python3-libcamera
    rpicam-apps
    rpicam-apps-lite

entre otros.

---

# 6. Actualizar el sistema

En nuestro caso hubo una actualización grande:

    sudo apt update
    sudo apt full-upgrade

Durante la actualización apareció:

    Pending kernel upgrade!
    
    Running kernel version:
      6.8.0-1057-raspi
    
    The currently running kernel version is not the expected kernel version
    6.8.0-1060-raspi.

Esto era normal.

La actualización había instalado un kernel nuevo, pero el sistema todavía estaba ejecutando el anterior.

Se debía reiniciar:

    sudo reboot

IMPORTANTE:

No ejecutar otra actualización encima si `dpkg` informa:

    dpkg was interrupted

En ese caso primero:

    sudo dpkg --configure -a

y después continuar con:

    sudo apt update
    sudo apt full-upgrade

---

# 7. Instalar libcamera 0.6

Después de agregar el PPA:

    sudo apt install libcamera-dev

Esto instaló automáticamente:

    libcamera-ipa
    libcamera0.6
    libcamera-dev

En nuestro caso:

    libcamera0.6
    0.6.0+rpt20251202-1ubuntu1~marco1

Comprobar:

    ldconfig -p | grep libcamera

Resultado correcto:

    libcamera.so.0.6 => /lib/aarch64-linux-gnu/libcamera.so.0.6
    libcamera.so => /lib/aarch64-linux-gnu/libcamera.so
    libcamera-base.so.0.6 => /lib/aarch64-linux-gnu/libcamera-base.so.0.6
    libcamera-base.so => /lib/aarch64-linux-gnu/libcamera-base.so

---

# 8. Comprobar headers de libcamera

Ejecutar:

    ls /usr/include/libcamera

Debe existir el directorio.

En nuestro caso:

    /usr/include/libcamera/libcamera/

contenía archivos como:

    camera.h
    camera_manager.h
    controls.h
    formats.h
    framebuffer.h
    libcamera.h
    stream.h
    version.h

---

# 9. Comprobar pkg-config

Ejecutar:

    find /usr -name 'libcamera.pc' -o -name 'LibcameraConfig.cmake' -o -name 'libcamera-config.cmake' 2>/dev/null

Encontramos:

    /usr/lib/aarch64-linux-gnu/pkgconfig/libcamera.pc

Aunque inicialmente:

    pkg-config --modversion libcamera

no encontraba el paquete.

Esto no terminó siendo un problema porque CMake encontró directamente la biblioteca instalada.

---

# 10. Comprobar las aplicaciones de cámara

La guía menciona `rpicam` como sucesor de los antiguos comandos `libcamera-*`.

Después de instalar los paquetes adecuados, la prueba importante fue:

    rpicam-still -o test.jpg

Esto abrió la cámara y mostró correctamente la imagen en el monitor de la Raspberry Pi.

RESULTADO:

    IMX219 funcionando correctamente con libcamera/rpicam.

Esto fue una prueba fundamental.

Si:

    rpicam-still -o test.jpg

muestra imagen, entonces:

- el cable MIPI está funcionando
- el sensor está funcionando
- el kernel detecta la cámara
- libcamera puede acceder a ella
- PiSP puede procesarla

---

# 11. `cam` y `qcam`

Inicialmente se intentó:

    cam -l

y posteriormente apareció:

    Command 'cam' not found

aunque el paquete correspondiente era `libcamera-tools`.

También se probó:

    qcam

pero apareció:

    qt.qpa.xcb: could not connect to display

Esto ocurría porque `qcam` intentaba utilizar el display gráfico de Qt y no tenía acceso al display desde esa sesión.

No fue necesario resolver esto.

La prueba:

    rpicam-still -o test.jpg

fue suficiente y funcionó.

---

# 12. Problema con ROS 2

El paquete originalmente utilizado por el proyecto era:

    v4l2_camera

El launch original utilizaba:

    package='v4l2_camera'
    executable='v4l2_camera_node'

Pero para una cámara MIPI de Raspberry Pi 5 la ruta correcta es:

    libcamera → camera_ros

Por lo tanto, se decidió utilizar:

    camera_ros

---

# 13. Comprobar paquetes ROS relacionados

Ejecutar:

    ros2 pkg list | grep camera

Inicialmente apareció:

    camera_calibration_parsers
    camera_info_manager
    v4l2_camera

Los otros paquetes de la guía no estaban instalados inicialmente.

---

# 14. Instalar `camera_ros`

La guía indicaba:

    sudo apt install ros-jazzy-camera-ros

El paquete disponible era:

    ros-jazzy-camera-ros

Sin embargo, había un problema importante con `ros-jazzy-libcamera`.

Inicialmente:

    ros-jazzy-libcamera

estaba instalado y requería:

    libcamera.so.0.7

mientras el sistema tenía:

    libcamera.so.0.6

Esto produjo:

    Could not load library dlopen error:
    libcamera.so.0.7: cannot open shared object file

Por lo tanto, se decidió NO utilizar la versión ROS de libcamera que esperaba 0.7.

---

# 15. Solución: utilizar `camera_ros` compilado contra libcamera 0.6

Se utilizó el repositorio:

    https://github.com/christianrauch/camera_ros

Workspace:

    ~/camera_ws

El repositorio estaba limpio y actualizado:

    cd ~/camera_ws/src/camera_ros
    
    git remote -v
    git status
    git branch --show-current
    git log -1 --oneline

Resultado:

    origin https://github.com/christianrauch/camera_ros.git
    
    On branch main
    Your branch is up to date with 'origin/main'.

---

# 16. Eliminar el paquete ROS incompatible

Se intentó:

    sudo apt remove ros-jazzy-libcamera

Si aparece:

    Package 'ros-jazzy-libcamera' is not installed

no hay problema.

Comprobar:

    dpkg -l | grep libcamera

La situación deseada para este procedimiento es que no aparezca:

    ros-jazzy-libcamera

---

# 17. Instalar las dependencias de desarrollo

Instalar:

    sudo apt install libcamera-dev

Esto proporciona:

    libcamera0.6
    libcamera-ipa
    libcamera-dev

---

# 18. Compilar `camera_ros`

Entrar al workspace:

    cd ~/camera_ws

Comprobar:

    ls

Debe existir:

    build
    install
    log
    src

Y:

    ls src

debe mostrar:

    camera_ros

Compilar:

    colcon build --packages-select camera_ros --symlink-install

Resultado correcto:

    Finished <<< camera_ros

y:

    Summary: 1 package finished

Después:

    source ~/camera_ws/install/setup.bash

Comprobar:

    ros2 pkg prefix camera_ros

Resultado:

    /home/andino/camera_ws/install/camera_ros

---

# 19. Comprobar que el componente usa libcamera 0.6

El ejecutable principal no mostraba directamente la dependencia mediante `ldd`, porque la biblioteca está en el componente.

Encontrar:

    find ~/camera_ws/install/camera_ros -type f | grep -E 'libcamera|\.so'

Resultado:

    ~/camera_ws/install/camera_ros/lib/libcamera_component.so

Comprobar:

    ldd ~/camera_ws/install/camera_ros/lib/libcamera_component.so | grep -E 'libcamera|not found'

Resultado correcto:

    libcamera.so.0.6 => /lib/aarch64-linux-gnu/libcamera.so.0.6
    libcamera-base.so.0.6 => /lib/aarch64-linux-gnu/libcamera-base.so.0.6

También aparecieron correctamente:

    libcamera_info_manager.so
    libcamera_calibration_parsers.so

y NO apareció:

    not found

Esto confirma que `camera_ros` está enlazado correctamente contra libcamera 0.6.

---

# 20. Lanzar `camera_ros`

En cada terminal:

    source /opt/ros/jazzy/setup.bash
    source ~/camera_ws/install/setup.bash

Luego:

    ros2 run camera_ros camera_node

Resultado correcto:

    libcamera v0.6.0+rpt20251202

Luego:

    cameras:
       0: imx219 (...)

Esto confirma que ROS 2 ve la cámara.

También apareció:

    Adding camera '...imx219@10'

y:

    Registered camera ... to CFE device /dev/media0 and ISP device /dev/media1

---

# 21. Configuración automática

Sin parámetros, `camera_ros` avisó:

    no camera selected, using default

y seleccionó:

    imx219

También seleccionó automáticamente:

    XRGB8888

y:

    800x600

Estos warnings no son errores.

Para evitar los warnings se pueden pasar parámetros explícitos.

---

# 22. Configuración que funciona

Para una primera prueba:

    ros2 run camera_ros camera_node \
        --ros-args \
        -p camera:=0 \
        -p width:=640 \
        -p height:=480

Esto utiliza:

    cámara 0
    640x480

---

# 23. Integración en el launch de Andino

El launch original utilizaba:

    v4l2_camera

y tenía:

    'image_size': [640, 480]
    'camera_frame_id': 'camera_link'
    'camera_info_url': ...

La versión nueva utiliza `camera_ros`.

Ejemplo mínimo funcional:

    from launch import LaunchDescription
    from launch_ros.actions import Node
    
    def generate_launch_description():
        return LaunchDescription([
            Node(
                package='camera_ros',
                executable='camera_node',
                name='camera',
                output='screen',
                parameters=[{
                    'camera': 0,
                    'width': 640,
                    'height': 480,
                }],
            )
        ])

IMPORTANTE:

Por ahora no se debe intentar resolver la calibración dentro de este paso.

Primero comprobar que la imagen llega a ROS.

---

# 24. Resultado final

La ejecución correcta produjo:

    libcamera v0.6.0+rpt20251202
    
    cameras:
       0: imx219
    
    configuring streams:
       800x600-XRGB8888/sRGB

Y luego:

    camera ... configured with 800x600-XRGB8888/sRGB stream

Esto significa que la cadena completa funciona:

    IMX219
       ↓
    MIPI CSI
       ↓
    RP1 CFE
       ↓
    PiSP
       ↓
    libcamera 0.6
       ↓
    camera_ros
       ↓
    ROS 2

---

# 25. Calibración

El único mensaje pendiente fue:

    Unable to open camera calibration file

con una ruta similar a:

    /home/andino/.ros/camera_info/imx219__base_axi_pcie_120000_rp1_i2c_88000_imx219_10_800x600.yaml

Esto NO significa que la cámara esté fallando.

Significa simplemente que todavía no existe el archivo YAML de calibración.

La calibración se hará posteriormente.

---

# 26. Verificar los topics

Con `camera_node` ejecutándose, abrir otra terminal:

    source /opt/ros/jazzy/setup.bash
    source ~/camera_ws/install/setup.bash

Ejecutar:

    ros2 topic list

Deberían aparecer:

    /camera/camera_info
    /camera/image_raw

También:

    ros2 topic hz /camera/image_raw

Esto permite comprobar que están llegando imágenes.

---

# 27. Estado final de la instalación

La configuración que funciona es:

    Ubuntu 24.04 Noble
    ROS 2 Jazzy
    Raspberry Pi 5
    IMX219
    camera_auto_detect=1
    libcamera 0.6
    libcamera-dev 0.6
    libcamera-ipa 0.6
    PPA marco-sonic/rasppios
    camera_ros compilado desde GitHub
    camera_ros enlazado contra libcamera.so.0.6

No utilizar:

    ros-jazzy-libcamera

si intenta cargar:

    libcamera.so.0.7

No utilizar:

    v4l2_camera

para esta cámara MIPI si `camera_ros` funciona correctamente.

---

# 28. Checklist para repetir en otro robot

## Sistema

    [ ] Ubuntu 24.04 64-bit
    [ ] ROS 2 Jazzy instalado
    [ ] Cámara conectada correctamente
    [ ] Usuario pertenece al grupo video

Comprobar:

    groups

---

## Cámara / kernel

    [ ] camera_auto_detect=1

Comprobar:

    sudo dmesg | grep -Ei "imx219|camera|csi|rp1|pisp"

Debe aparecer:

    Using sensor imx219

y:

    Registered ... /dev/video0

---

## PPA

    [ ] PPA marco-sonic agregado

Comando:

    sudo add-apt-repository ppa:marco-sonic/rasppios
    sudo apt update

Comprobar:

    apt policy libcamera-tools

Debe ofrecer:

    0.6.0+rpt20251202-1ubuntu1~marco1

---

## libcamera

    sudo apt install libcamera-dev

Comprobar:

    ldconfig -p | grep libcamera

Debe aparecer:

    libcamera.so.0.6
    libcamera-base.so.0.6

---

## Cámara independiente de ROS

Probar:

    rpicam-still -o test.jpg

Si muestra imagen:

    [OK] cámara funcionando

---

## camera_ros

Workspace:

    ~/camera_ws

Repositorio:

    https://github.com/christianrauch/camera_ros

Compilar:

    cd ~/camera_ws
    colcon build --packages-select camera_ros --symlink-install

Activar:

    source /opt/ros/jazzy/setup.bash
    source ~/camera_ws/install/setup.bash

---

## Dependencias

Comprobar:

    ldd ~/camera_ws/install/camera_ros/lib/libcamera_component.so | grep -E 'libcamera|not found'

Debe aparecer:

    libcamera.so.0.6
    libcamera-base.so.0.6

Y NO:

    not found

---

## Prueba ROS

    ros2 run camera_ros camera_node

Debe detectar:

    0: imx219

---

## Prueba de topics

En otra terminal:

    source /opt/ros/jazzy/setup.bash
    source ~/camera_ws/install/setup.bash
    
    ros2 topic list

Y:

    ros2 topic hz /camera/image_raw

---

# 29. Comandos de diagnóstico rápidos

## ¿El kernel ve la cámara?

    sudo dmesg | grep -Ei "imx219|camera|csi|rp1|pisp"

## ¿libcamera instalado?

    ldconfig -p | grep libcamera

## ¿Qué versión ofrece el PPA?

    apt policy libcamera-tools

## ¿Qué cámaras ve libcamera?

    rpicam-still --list-cameras

## ¿La cámara produce imagen?

    rpicam-still -o test.jpg

## ¿Está el usuario en video?

    groups

## ¿Qué versión de camera_ros está instalada?

    ros2 pkg prefix camera_ros

## ¿Está compilado?

    ldd ~/camera_ws/install/camera_ros/lib/libcamera_component.so | grep -E 'libcamera|not found'

## ¿ROS ve la cámara?

    ros2 run camera_ros camera_node

## ¿ROS publica imágenes?

    ros2 topic list
    ros2 topic hz /camera/image_raw

---

# 30. Regla importante para futuras instalaciones

No cambiar varias cosas simultáneamente.

El orden recomendado es:

    1. Kernel detecta IMX219
    2. rpicam funciona
    3. libcamera 0.6 instalado
    4. camera_ros compilado contra 0.6
    5. camera_ros detecta IMX219
    6. ROS publica /camera/image_raw
    7. Integrar en andino_bringup
    8. Calibrar cámara
    9. Optimizar formato/resolución

Si un paso falla, detenerse ahí.

No avanzar modificando otros componentes hasta determinar qué capa está fallando.