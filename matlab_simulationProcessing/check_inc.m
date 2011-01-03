cd 'no_comm_real/2robots_nc_envT/envT/robot2'
goodcnt=[];
comcnt=[];
passcnt=[];
for nm=dir('proc*out.mat')'
nm.name
load(nm.name);
state=1;
good=0;
bad=0;
competing=0;
AVOID=0;
NAV=1;

for r=1:2
for i=2:size(out,2)
	[good out(i) bad];
	if out(r,i) == 0
		good++;
	endif
	if out(r,i)==2 || state==AVOID ||out(r,i)==3
		if out(r,i)==3
			competing=1;
		endif
		bad++;
		if state == NAV 
			%disp 'going to avoid ',i	
			state=AVOID;
			goodcnt=[goodcnt;i good];
			good=0;
		elseif good > 0 && (out(r,i)==3 || out(r,i)==2)
			bad=good+bad;
			good=0;
		endif
		
	endif
	if state==AVOID && good > 100
		%disp 'going to nav ',i	
		state=NAV;
		if competing == 0 passcnt=[passcnt;i bad];
		else comcnt=[comcnt;i bad];
		endif
		bad=0;
		competing=0;
	endif
endfor
endfor
goodcnt
comcnt
passcnt
endfor

cd '/Users/manderson/testing_sim'
%need nominal incident count and duration

%at least 10 seconds before new incident