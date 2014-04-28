J=rand(6,10);
Mx=[eye(3),zeros(3);zeros(3),0.5*eye(3)];
Mq=[0.2*eye(3),zeros(3,7);zeros(7,3),eye(7)];

Jw=Mx*J*Mq;
Ji=Mq*pinv(Jw)*Mx;

dx=rand(6,1);

% Gewichtung im Taskraum erzeugt keine Verstärkung
dy=J*Ji*dx;
fprintf ('\nweighted Jacobian solver: J*J#dx-dx: %f\n', norm(dy-dx));
fprintf ('gain: \n');
(dy ./ dx)'

% Null-Space projection
N=Mq * ( eye(size(Jw,2)) - pinv(Jw)*Jw );
fprintf ('null space projection norm: %f\n', norm(J*N));

% truncated and 
Jt=Jw(1:3,:);
Jz=[Jt; zeros(size(Jt))];
Jzi=pinv(Jz);
fprintf ('difference of truncated and zeroed Jacobian: %f\n', norm(Jzi(:,1:3) - pinv(Jt)));