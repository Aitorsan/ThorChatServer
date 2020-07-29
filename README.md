# ThorChatServer
This is a simple desktop chat app that contains a barebones server and a client. The intend is to improve it over the time to make it a little bit more usefull and learn in the process.

## Dependencies to develop 

At the moment is only the project is meant to be develop under Linux. You will need to install qt first to be able to run build the client and develop:

```bash
sudo apt-get install qt5-default
```
You will also need to install qtwebkit:

```bash
sudo apt-get install libqt5webkit5-dev
```
## Workflow

There are 3 branches:

- master : code that is working but contains debug info.
- develop: code that is in develop and contain debug info.
- release: contain code without any debug info.