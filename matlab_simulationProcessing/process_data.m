loaddata

r1=delbad(r1,4,5);
r2=delbad(r2,4,5);

start1=r1(1,2);
start2=r2(1,2);

if (start1 < start2)
start=start1
else
start=start2
endif

r1(:,2)=r1(:,2)-start;
r2(:,2)=r2(:,2)-start;

end1=r1(end,2);
end2=r2(end,2);

if (end1 > end2)
endnum=end1
else	
endnum=end2
endif

r1idx=1
r2idx=1
g1idx=1
g2idx=1

sr1=size(r1)
sr2=size(r2)
sg1=size(g1)
sg2=size(g2)
rowcnt=1

data = []

idx=1;
while (tm(idx,7) <.9)
	idx++;
endwhile
ending=tm(idx,5)


for time=1:100:ending
	time;
	%print output for this time step
	temp=[time r1(r1idx,:) r2(r2idx,:) g1(g1idx,:) g2(g2idx,:)];
	data(rowcnt,:) = [time r1(r1idx,:) r2(r2idx,:) g1(g1idx,:) g2(g2idx,:)];
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
	while (g1(g1idx,1) < time && g1idx < sg1(1,1) )
		g1idx=g1idx+1;
	endwhile
	if g1idx > 1 g1idx--;endif
	while (g2(g2idx,1) < time && g2idx < sg2(1,1) )
		g2idx=g2idx+1;
	endwhile
	if g2idx > 1 g2idx--;endif

endfor

getobs
