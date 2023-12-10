# pullfilec
这是一个用于Windows命令行的文件选择器，经过测试，Windows7及以上都可以正常使用。

当您在命令行当中输入“start /wait pullfilec xxx\xxx.txt ”时，程序会运行，程序允许您将您的文件拖入窗口，可以多次拖入多个文件，当您点击确认按键之后，程序会将您选择的文件路径输入到您指定的文本文档内。
![pull](https://github.com/bdth-7777777/pullfilec/assets/142554252/fcb8790e-a914-4ba9-a76c-21f289f323f6)

例如我指定程序将路径输入到text\path.txt当中

start /wait pullfilec text\path.txt

则程序运行完毕之后，文本文档当中就会有我选择的文件路径。
![8](https://github.com/bdth-7777777/pullfilec/assets/142554252/50ae48e6-8caa-42fc-a2d0-71ff60474b13)

需要您注意的是，程序只有在命令行当中被调用时才能正常运行，并且，您必须使用示例的命令方式才能使程序正常运行。
