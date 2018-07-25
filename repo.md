# Repo服务器搭建(使用已有repo工程搭建repo代码服务器)

[参考文章:把已有的repo工程提交到服务器](http://nicekwell.net/blog/20171112/ba-yi-you-de-repogong-cheng-ti-jiao-dao-fu-wu-qi.html)

### 服务器配置环境简单介绍

- 服务器A 172.1.2.8用于存放repo的元数据,进行代码托管
- 服务器B 172.1.2.7用于操作服务器A上的元数据(也可以在A上直接操作)
- 客户端C 172.1.2.6是正常使用的客户端,普通开发用户

## manifest.xml文件介绍

### manifest中头部会有如下信息(原始内容如下)

- remote表示远程仓库,可以写多个
- default表示使用默认同步远程哪个仓库和分支
- revision表示远程仓库(manifest)的分支或标签
- sync-j指定同步线程数

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="aosp"/>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="rk"/>
	<default remote="aosp" revision="refs/tags/android-7.1.2_r6" sync-j="4"/>
```

现在想要将repo代码放到服务器172.1.2.8上路径为/home/anonymous/androidprj下,起名为myandroid

将默认同步服务器地址改为myandroid,默认同步master分支

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="aosp"/>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="rk"/>
	<remote fetch="ssh://anonymous@172.1.2.8:/home/anonymous/androidprj/" name="myandroid"/>
	<default remote="myandroid" revision="master" sync-j="24"/>
```

### 工程名

```xml
<project name="android/RKDocs" path="RKDocs"/>
<project name="android/RKTools" path="RKTools"/>
<project name="android/rk/u-boot" path="u-boot"/>
```

- path	本地相对路径(代码同步后在本地的路径),可以不指定,不指定的话表示和name相同
- name	远程相对与remote地址的路径(在远程仓库的路径)

在远程服务器(172.1.2.8)建立一个manifests.git

	cd /home/anonymous/androidprj
	git init --bare manifest.git

在本地(172.1.2.7)解压SDK代码(含.repo目录),删除代码中所有.git目录

	tar xvf rk3288_mid_7.1_180613.tgz
	cd rk3288_mid_7.1_180613/
	find . -name ".git" | xargs rm -rfv

拷贝当前的manifests文件到manifest目录中(172.1.2.7)

	cd rk3288_mid_7.1_180613/
	git clone ssh://172.1.2.8:/home/anonymous/androidprj/manifest.git
	cp .repo/manifests/rk3288_tablet_nougat_release.xml manifest/default.xml

其中需要将default.xml修改为如下

```xml
<remote fetch="ssh://anonymous@172.1.2.8:/home/anonymous/androidprj/" name="myandroid"/>
<default remote="myandroid" revision="master" sync-j="24"/>
```

去掉default.xml中的version信息(python3)

	./del_remote_version.py default.xml

将本地修改default.xml提交到到远程服务器

	git add default.xml
	git commit -m 'add default.xml'
	git push

## 远程服务器(172.1.2.8)批量创建工程

- 将上面的default.xml拷贝到远程服务器
- 使用提供脚本getnames_and_create_project.py批量创建工程

服务器(172.1.2.8)上批量创建工程

	cd /home/anonymous/androidprj
	./getnames_and_create_project.py default.xml

服务器上回产生相应的目录,大概如下

	android  default.xml  getnames_and_create_project.py  manifest.git  rk

## 本地(172.1.2.7)提交本地SDK代码

	cd rk3288_mid_7.1_180613/
	./getnames_and_init_push_git_proj.py default.xml

其中需要修改脚本getnames_and_init_push_git_proj.py如下

	remote = 'anonymous@172.1.2.8:/home/anonymous/androidprj'

## 新客户端下载代码

客户端(172.1.2.6)下载代码

	repo init -u anonymous@172.1.2.8:/home/anonymous/androidprj/manifest.git --repo-url https://mirrors.tuna.tsinghua.edu.cn/git/git-repo
	repo sync

或是将REPO_URL写在repo脚本中(/usr/bin/repo)就不需要后面指定url

其中--repo-url指定的是下载repo工具的所有源代码的地址(本地的repo只是一个引导文件)

```python
import os
REPO_URL = os.environ.get('REPO_URL', None)
if not REPO_URL:
	REPO_URL = 'https://mirrors.tuna.tsinghua.edu.cn/git/git-repo'
REPO_REV = 'stable'
```

修改后下载命令

	repo init -u anonymous@172.1.2.8:/home/anonymous/androidprj/manifest.git
	repo sync

## Repo新建分支,重复利用代码

新建一个分支,定制需要的工程(project),比如只要kernel

在(172.1.2.7)客户端上操作,给manifest里添加一个分支假设名叫kernel_only

	cd rk3288_mid_7.1_180613/manifest
	git checkout -b kernel_only

修改default.xml内容如下,只保留一个project(kernel)
```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="aosp"/>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="rk"/>
	<remote fetch="ssh://anonymous@172.1.2.8:/home/anonymous/androidprj/" name="myrkrepo"/>
	<default remote="myrkrepo" revision="kernel_only" sync-j="24"/>
	<project name="rk/kernel" path="kernel"/>
</manifest>
```

在(172.1.2.7)客户端上操作,提交manifest

	git push origin kernel_only:kernel_only

在(172.1.2.7)上提交相应工程(和manifest里project name一致,比如这里的rk/kernel)

	cd kernel
	rm -rf .git && git init && git remote add origin anonymous@172.1.2.8:/home/anonymous/androidprj/rk/kernel && git add . -f && git commit -m "init"
	git push -u origin master:kernel_only

在(172.1.2.6)客户端下载代码

	repo init -u anonymous@172.1.2.8:/home/anonymous/androidprj/manifest.git -b kernel_only
	repo sync

## 修改Repo manifest文件内容达到控制工程

在(172.1.2.7)客户端上操作,修改manifest的master分支内容如下
删除不需要工程后,假设只保留了RKDocs
```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="aosp"/>
	<remote fetch="ssh://git@www.rockchip.com.cn/gerrit/" name="rk"/>
	<remote fetch="ssh://anonymous@172.1.2.8:/home/anonymous/androidprj/" name="myrkrepo"/>
	<default remote="myrkrepo" revision="master" sync-j="24"/>
	<project name="android/RKDocs" path="RKDocs"/>
</manifest>
```

在(172.1.2.7)上将修改好的文件更新到服务器上

	cd rk3288_mid_7.1_180613/manifest
	git push

此时客户端(172.1.2.6)更新代码就只有doc了,本地原有的代码将被删除

	repo init -u anonymous@172.1.2.8:/home/anonymous/androidprj/manifest.git
	repo sync
