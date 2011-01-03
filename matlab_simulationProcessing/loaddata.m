
robot=1

rfile = strcat('no_comm_real/2robots_nc_env',type,'/env',type,'/robot',num2str(robot),'/proc_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc.txt')
if ( exist(strcat(['r',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r1 rfile'
	cmd1 = strcat(['grep "^[0123]" ',rfile,' > r',num2str(robot)],'_',num2str(run),'_',type)
	system(cmd1)
endif

gfile = strcat('no_comm_real/2robots_nc_env',type,'/env',type,'/robot',num2str(robot),'/log_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc')
if ( exist(strcat(['g',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r1 gfile'
	cmd2 = strcat(['grep "Goal = " ',gfile,' |sed -f replace_goals.sed > g',num2str(robot),'_',num2str(run),'_',type])
	system(cmd2)
endif

r1=load(strcat(['r',num2str(robot),'_',num2str(run),'_',type]));
g1=load(strcat(['g',num2str(robot),'_',num2str(run),'_',type]));

robot=2
rfile = strcat('no_comm_real/2robots_nc_env',type,'/env',type,'/robot',num2str(robot),'/proc_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc.txt')

if ( exist(strcat(['r',num2str(robot),'_',num2str(run),'_',type])) ~= 2)
	disp 'processing r2 rfile'
	cmd1 = strcat(['grep "^[0123]" ',rfile,' > r',num2str(robot),'_',num2str(run),'_',type])
	system(cmd1)
endif

gfile = strcat('no_comm_real/2robots_nc_env',type,'/env',type,'/robot',num2str(robot),'/log_robot',num2str(robot),'_run',num2str(run),'_env',type,'_nc')
if ( exist(strcat(['g',num2str(robot),'_',num2str(run)],'_',type)) ~= 2)
	disp 'processing r2 gfile'
	cmd2 = strcat(['grep "Goal = " ',gfile,' |sed -f replace_goals.sed > g',num2str(robot),'_',num2str(run),'_',type])
	system(cmd2)
endif

r2=load(strcat(['r',num2str(robot),'_',num2str(run),'_',type]));
g2=load(strcat(['g',num2str(robot),'_',num2str(run),'_',type]));

disp 'done loading'

['no_comm_real/2robots_nc_env',type,'/matlab/run',num2str(run),'/time_statsdata.csv']

tm=load(['no_comm_real/2robots_nc_env',type,'/matlab/run',num2str(run),'/time_statsdata.csv']);
size(r1)
size(r2)
size(g1)
size(g2)
name=strcat(['m',num2str(robot),'_',num2str(run),'.mat'])
save(name);