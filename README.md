# pullfilec
这是一个用于Windows命令行的文件选择器，经过测试，Windows7及以上都可以正常使用。

当您在命令行当中输入“start /wait pullfilec -o xxx\xxx.txt ”时，程序会运行，程序允许您将您的文件拖入窗口，可以多次拖入多个文件，当您点击确认按键之后，程序会将您选择的文件路径输入到您指定的文本文档内。
![QQ截图20231211010126](https://github.com/bdth-7777777/pullfilec/assets/142554252/ab5fd512-125b-44c9-af53-4ba1fc8bcd79)


例如我指定程序将路径输入到text\path.txt当中

start /wait pullfilec -o text\path.txt

则程序运行完毕之后，文本文档当中就会有我选择的文件路径。
![image](https://github.com/bdth-7777777/pullfilec/assets/142554252/3a5c65f8-4e8a-451c-9622-2b749f6afc2f)


需要您注意的是，如果您不使用-o [txt路径] ,那么路径会默认输入到filepath.txt
