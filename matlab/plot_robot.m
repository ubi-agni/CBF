 
function plot_robot(q)
	f = fk_full(q);
	plot([-3 3], [-3, 3], ".",  [0 f(1,:)], [0 f(2,:)], "-", f(1,3), f(2,3), "*");
end