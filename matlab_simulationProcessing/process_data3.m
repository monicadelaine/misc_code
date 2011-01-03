loaddata3

r1=delbad(r1,4,5);
r2=delbad(r2,4,5);
r3=delbad(r3,4,5);
disp 'done removing bad'
start1=r1(1,2);
start2=r2(1,2);
start3=r3(1,2);

if (start1 < start2 && start1 < start3)
start=start1
elseif (start2 < start1 && start2 < start3)
start=start2
else 
start=start3
endif

r1(:,2)=r1(:,2)-start;
r2(:,2)=r2(:,2)-start;
r3(:,2)=r3(:,2)-start;

end1=r1(end,2);
end2=r2(end,2);
end3=r3(end,2);

if (end1 < end2 && end1 < end3)
endnum=end1
elseif (end2 < end1 && end2 < end3)
endnum=end2
else 
endnum=end3
endif

r1idx=1
r2idx=1
r3idx=1
g1idx=1
g2idx=1
g3idx=1

sr1=size(r1)
sr2=size(r2)
sr3=size(r3)
sg1=size(g1)
sg2=size(g2)
sg3=size(g3)
rowcnt=1

data = []

idx=1;
while (tm(idx,7) <.9)
	idx++;
endwhile
ending=tm(idx,5);

for time=1:100:ending
	time;
	%print output for this time step
	data(rowcnt,:) = [time r1(r1idx,:) r2(r2idx,:) g1(g1idx,:) g2(g2idx,:) r3(r3idx,:) g3(g3idx,:)];
	rowcnt=rowcnt+1;
	
	%read both lists until next break
	while (r1(r1idx,2) < time && r1idx < sr1(1,1) )
		r1idx=r1idx+1;
	endwhile
	if r1idx > 1 r1idx--;endif
	while (r2(r2idx,2) < time && r2idx < sr2(1,1) )
		r2idx=r2idx+1;
	endwhile

	if r2idx > 1 r2idx--;endif

	while (r3(r3idx,2) < time && r3idx < sr3(1,1) )
		r3idx=r3idx+1;
	endwhile
	if r3idx > 1 r3idx--;endif
	while (g1(g1idx,1) < time && g1idx < sg1(1,1) )
		g1idx=g1idx+1;
	endwhile
	if g1idx > 1 g1idx--;endif
	while (g2(g2idx,1) < time && g2idx < sg2(1,1) )
		g2idx=g2idx+1;
	endwhile
	if g2idx > 1 g2idx--;endif
	while (g3(g3idx,1) < time && g3idx < sg3(1,1) )
		g3idx=g3idx+1;
	endwhile
	if g3idx > 1 g3idx--;endif

endfor

getobs3_new
