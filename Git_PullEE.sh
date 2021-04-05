#! /bin/bash


#判断命令是否执行成功
#${1}:执行的命令语句
function Judge_Order(){
	local status=${?}
	if [ ${#} -eq 3 ];then
		#判断上一条命令的返回值是否为0，若为0则执行成功，若不为0则执行失败
		if [ ${status} -eq 0 ];then
			echo -e "[\033[32m$(date +"%Y-%m-%d %T") Info\033[0m]  ""\033[32m${1}\033[0m"
		else 
			echo -e "[\033[33m$(date +"%Y-%m-%d %T") Warning\033[0m]  ""\033[33m${2}\033[0m"
			if [ "${3}" -eq 0 ];then
				exit 127
			fi
		fi	
	else
		echo -e "[\033[31m$(date +"%Y-%m-%d %T") Error\033[0m] ""\033[31mIncorrect function parameter input, the \"Judge_Order()\" is about to terminate execution!\033[0m"
		return 80
	fi
}
function Main()
{
	#=======================================================
	#参数更改
	repository=git@gitee.com:ZC335134701/QT_Demo.git
	localDir=$(pwd)
	localDirName=${localDir##*/}
	#=======================================================
	git --version >/dev/null 2>&1
	Judge_Order "Git已安装!" "Git未安装!" 0
	if [ ! -d ${localDir}/.git ];then
		git init >/dev/null 2>&1
		Judge_Order "git init成功!" "git init失败!" 0
	fi
	#添加远程仓库 git remote add <名称> <URL>
	git remote add origin ${repository} >/dev/null 2>&1
	Judge_Order "添加远程仓库成功!" "添加远程仓库失败,远程仓库已存在!" 1
	#解决Permission denied的问题
	eval "$(ssh-agent -s)" ssh-add
	#判断本地有无${localDirName}分支,若没有则创建本地${localDirName}分支,并创建提交文件README.md
	$(git branch |grep -q "${localDirName}")
	if [ ${?} -eq 0 ];then
		git checkout ${localDirName}  >/dev/null 2>&1
		Judge_Order "切换至${localDirName}分支成功!" "切换至${localDirName}分支失败!" 0
	elif [ ${?} -eq 1 ];then
		git checkout -b ${localDirName}  >/dev/null 2>&1
		Judge_Order "创建${localDirName}分支成功!" "创建${localDirName}分支失败!" 0
	fi
	#判断本地分支与远程分支是否关联,如果没有则创建或建立连接(强调本地分支与远程分支同名)
	$(git branch -a |grep -q "remotes/origin/${localDirName}")
	if [ ${?} -ne 0 ];then
		#拉取远程分支到本地分支,如果拉取不成功则强制推送本地分支到远程并创建同名分支
		git fetch origin ${localDirName}
		if [ ${?} -eq 0 ];then
			echo -e "[\033[32m$(date +"%Y-%m-%d %T") Info\033[0m]  ""\033[32m拉取远程${localDirName}分支成功!\033[0m"
			test -f $(pwd)/Git_Pull.sh && \
				echo -e "[\033[32m$(date +"%Y-%m-%d %T") Info\033[0m]  ""\033[32m删除文件$(pwd)/Git_Pull.sh!\033[0m" && \
				rm -rf Git_Pull.sh
			git pull origin ${localDirName} >/dev/null 2>&1
			Judge_Order "获取最新的${localDirName}分支成功" "获取最新的${localDirName}分支失败!" 0
		else
			echo "# ${localDirName}分支" >> README.md
			git add README.md >/dev/null 2>&1
			git commit -m "${localDirName}分支创建并提交!" >/dev/null 2>&1
			Judge_Order "本地提交README.md成功!" "本地提交README.md失败!" 0 
			#如果远程不存在与本地分支同名的分支则强制推送本地分支到远程并创建同名分支
			git push -u origin ${localDirName} >/dev/null 2>&1
			Judge_Order "创建并推送远程${localDirName}分支成功!" "创建并推送远程${localDirName}分支失败!" 0 
		fi
	else
		git pull origin ${localDirName} >/dev/null 2>&1
		Judge_Order "获取最新的${localDirName}分支成功" "获取最新的${localDirName}分支失败!" 0
	fi
}

Main
