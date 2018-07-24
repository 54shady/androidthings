# Repo服务器搭建(使用已有repo工程搭建repo代码服务器)

[参考文章:把已有的repo工程提交到服务器](http://nicekwell.net/blog/20171112/ba-yi-you-de-repogong-cheng-ti-jiao-dao-fu-wu-qi.html)

## manifest.xml文件介绍

### manifest中头部会有如下信息(原始内容如下)

- remote表示原厂仓库,可以写多个
- default 表示使用默认同步远程哪个仓库
- revision表示原厂仓库的分支或标签
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
