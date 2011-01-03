r1obs=0;
r2obs=0;
r1nav=0;
r2nav=0;
r1com=0;
r2com=0;
r1pass=0;
r2pass=0;
r1stop=0;
r2stop=0;
r1oth=0;
r2oth=0;
NAV=0;
OBS=1;
PASS=2;
COMP=3;
STOP=4;
OTH=5;

out=[];

for row=1:rowcnt-1
		dist = sqrt((data(row,5)-data(row,11)).^2+(data(row,12)-data(row,6)).^2);
		gdist = sqrt((data(row,15)-data(row,20)).^2+(data(row,16)-data(row,21)).^2);
		%r1
		if ((data(row,2) == 2 || data(row,2) == 1) && ((dist < 500 && gdist < 1)))
			out(1,row)=COMP;
			r1com=r1com+1;
		elseif ((data(row,2) == 2 || data(row,2) == 1) && ((dist < 500 )))
			out(1,row)=PASS;
			r1pass=r1pass+1;
		elseif (data(row,2) == 2 || data(row,2) == 1)
			out(1,row)=OBS;
			r1obs=r1obs+1;
		elseif (data(row,2) == 3 )
			out(1,row)=NAV;
			r1nav=r1nav+1;
		elseif (data(row,2) == 0 )
			out(1,row)=STOP;
			r1stop=r1stop+1;
		else
			out(1,row)=OTH;
			r1oth=r1oth+1;
		endif
		%r2
		if ((data(row,8) == 2 || data(row,8) == 1) && ((dist < 500 && gdist < 1)))
			out(2,row)=COMP;
			r2com=r2com+1;
		elseif ((data(row,8) == 2 || data(row,8) == 1)&& ((dist < 500 ) ))
			out(2,row)=PASS;
			r2pass=r2pass+1;
		elseif (data(row,8) == 2 || data(row,8) == 1)
			out(2,row)=OBS;
			r2obs=r2obs+1;
		elseif (data(row,8) == 3 )
			out(2,row)=NAV;
			r2nav=r2nav+1;
		elseif (data(row,8) == 0 )
			out(2,row)=STOP;
			r2stop=r2stop+1;
		else
			out(2,row)=OTH;
			r2oth=r2oth+1;
		endif
endfor

save([rfile(1:end-4),'_out.mat'],'out');
save([rfile(1:end-4),'_all.mat']);

r1obs
r2obs
r1nav
r2nav
r1com
r2com
r1pass
r2pass
r1stop
r2stop
r1oth
r2oth