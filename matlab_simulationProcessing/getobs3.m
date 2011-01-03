r1obs=0;
r2obs=0;
r1nav=0;
r2nav=0;
r1com=0;
r2com=0;
r1pass=0;
r2pass=0;


for row=1:rowcnt-1

	if ( data(row,2) == 3 && data(row,8) == 3 && && data(row,XXX) == 3) 
		r1nav=r1nav+1;
		r2nav=r2nav+1;
		r3nav=r3nav+1;
	else
		dist1 = sqrt((data(row,5)-data(row,11)).^2+(data(row,12)-data(row,6)).^2);
		dist2 = sqrt((data(row,5)-data(row,28)).^2+(data(row,29)-data(row,6)).^2);
		dist3 = sqrt((data(row,28)-data(row,11)).^2+(data(row,12)-data(row,29)).^2);
		gdist1 = sqrt((data(row,15)-data(row,20)).^2+(data(row,16)-data(row,21)).^2);
		gdist2 = sqrt((data(row,15)-data(row,32)).^2+(data(row,16)-data(row,33)).^2);
		gdist3 = sqrt((data(row,32)-data(row,20)).^2+(data(row,33)-data(row,21)).^2);
		if ((dist1 < 500 && dist2 > 500 && dist3 > 500))
			% one and two but not three
			if (gdist1 < 1)
				r1com=r1com+1;
				r2com=r2com+1;
			else
				r1pass=r1pass+1;
				r2pass=r2pass+1;
			endif
			if (data(row,24) == 2 ) 
				r3obs=r3obs+1;
			else
				r3nav=r3nav+1;
			endif
			
		elseif (dist1 > 500 && dist2 < 500 && dist3 > 500)
			%one and three but not two
			if (gdist2 < 1)
				r1com=r1com+1;
				r3com=r3com+1;
			else
				r1pass=r1pass+1;
				r3pass=r3pass+1;
			endif
			if (data(row,8) == 2 ) 
				r2obs=r2obs+1;
			else
				r2nav=r2nav+1;
			endif
		elseif (dist1 > 500 && dist2 > 500 && dist3 < 500)
		%two and three but not one
			%one and three but not two
			if (gdist3 < 1)
				r2com=r2com+1;
				r3com=r3com+1;
			else
				r2pass=r2pass+1;
				r3pass=r3pass+1;
			endif
			if (data(row,2) == 2 ) 
				r1obs=r1obs+1;
			else
				r1nav=r1nav+1;
			endif
		else
			if (gdist1 < 1 && gdist2 > 1 && gdist3 > 1)
				r1com=r1com+1;
				r2com=r2com+1;
				r3pass=r3pass+1;
			elseif (gdist1 > 1 && gdist2 < 1 && gdist3 > 1)
				r1com=r1com+1;
				r3com=r3com+1;
				r2pass=r2pass+1;
			elseif (gdist1 > 1 && gdist2 < 1 && gdist3 > 1)
				r2com=r2com+1;
				r3com=r3com+1;
				r1pass=r1pass+1;
			else
				cnt=0
				if gdist1 > 1
					cnt++
				endif
				if gdist2 > 1
					cnt++
				endif
				if gdist3 > 1
					cnt++
				endif
				if (cnt > 2)
					r1pass=r1pass+1;
					r2pass=r2pass+1;
					r3pass=r3pass+1;
				else
					r1com=r1com+1;
					r2com=r2com+1;
					r3com=r3com+1;
				endif
			endif			
		endif
	endif
endfor

r1obs
r2obs
r3obs
r1nav
r2nav
r3nav
r1com
r2com
r3com
r1pass
r2pass
r3pass
