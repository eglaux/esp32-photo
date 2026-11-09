# ESP32 photo capture

Característiques

- Capturar imatge des de la càmera del ESP32
- Emmagatzemar les imatges a la memòria SD
- HTTP API per manipular el dispositiu
- GUI feta amb frontal web per interactuar amb l'API
- Connectivitat via wifi access point


Instal·lació:

- Formatar SD FAT32
- Copiar la carpeta assets/web a l'arrel de la SD
- Pujar software a esp32
- Connectar a acces point 'ESP32-CAM' amb la contrasenya definida a main.c
- Accedir via HTTP (port 80) a l'ESP32 normalment IP 192.168.4.1


API endpoints

- 'capture': per capturar una imatge i desar-la a la SD
- 'list': llista totes les imatges desades
- 'download': descarrega una imatge indicada pel paràmetre 'file'


*Observacions: projecte elaborat amb assistència d'IA generativa.*
