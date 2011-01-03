
robot=1
rfile = strcat('no_comm_real/3robots_env',type,'_nc/env',type,'/robot',num2str(robot),'_env',type,'_nc/proc_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc.txt')
if ( exist(strcat(['rr',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r1 rfile'
	cmd1 = strcat(['grep "^[0123]" ',rfile,' > rr',num2str(robot),'_',num2str(run),'_',type])
	system(cmd1)
endif

gfile = strcat('no_comm_real/3robots_env',type,'_nc/env',type,'/robot',num2str(robot),'_env',type,'_nc/log_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc')
if ( exist(strcat(['gg',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r1 gfile'
	cmd2 = strcat(['grep "Goal = " ',gfile,' |sed -f replace_goals.sed > gg',num2str(robot),'_',num2str(run),'_',type])
	system(cmd2)
endif

r1=load(strcat(['rr',num2str(robot),'_',num2str(run),'_',type]));
g1=load(strcat(['gg',num2str(robot),'_',num2str(run),'_',type]));

robot=2
rfile = strcat('no_comm_real/3robots_env',type,'_nc/env',type,'/robot',num2str(robot),'_env',type,'_nc/proc_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc.txt')

if ( exist(strcat(['rr',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r2 rfile'
	cmd1 = strcat(['grep "^[0123]" ',rfile,' > rr',num2str(robot),'_',num2str(run),'_',type])
	system(cmd1)
endif

gfile = strcat('no_comm_real/3robots_env',type,'_nc/env',type,'/robot',num2str(robot),'_env',type,'_nc/log_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc')
if ( exist(strcat(['gg',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r2 gfile'
	cmd2 = strcat(['grep "Goal = " ',gfile,' |sed -f replace_goals.sed > gg',num2str(robot),'_',num2str(run),'_',type])
	system(cmd2)
endif

r2=load(strcat(['rr',num2str(robot),'_',num2str(run),'_',type]));
g2=load(strcat(['gg',num2str(robot),'_',num2str(run),'_',type]));


robot=3
rfile = strcat('no_comm_real/3robots_env',type,'_nc/env',type,'/robot',num2str(robot),'_env',type,'_nc/proc_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc.txt')

if ( exist(strcat(['rr',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r3 rfile'
	cmd1 = strcat(['grep "^[0123]" ',rfile,' > rr',num2str(robot),'_',num2str(run),'_',type])
	system(cmd1)
endif

gfile = strcat('no_comm_real/3robots_env',type,'_nc/env',type,'/robot',num2str(robot),'_env',type,'_nc/log_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc')
if ( exist(strcat(['gg',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r2 gfile'
	cmd2 = strcat(['grep "Goal = " ',gfile,' |sed -f replace_goals.sed > gg',num2str(robot),'_',num2str(run),'_',type])
	system(cmd2)
endif

r3=load(strcat(['rr',num2str(robot),'_',num2str(run),'_',type]));
g3=load(strcat(['gg',num2str(robot),'_',num2str(run),'_',type]));


tm=load(['no_comm_real/3robots_envT_nc/matlab/run',num2str(run),'/time_statsdata.csv']);

disp 'done loading'
size(r1)
size(r2)
size(r3)
size(g1)
size(g2)
size(g3)

name=strcat(['mm',num2str(robot),'_',num2str(run),'.mat'])
save(name);