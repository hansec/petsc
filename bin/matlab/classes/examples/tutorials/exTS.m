%%
% Solves a linear system where the user manages the mesh--solver interactions
%
%   Set the Matlab path and initialize PETSc
path(path,'../../')
PetscInitialize({'-ts_monitor','-snes_monitor','-ksp_monitor','-ts_theta_theta','1'});
%%
%   Open a viewer to display PETSc objects
viewer = PetscViewer();
viewer.SetType('ascii');
%%
%   Create work vector for nonlinear solver and location for solution
x = PetscVec();
x.SetType('seq');
x.SetSizes(10,10);
x(:) = 1:10;
x(:) = x(:) .* x(:).* x(:);
%%
%  Create a matrix for the Jacobian for Newton method
mat = PetscMat();
mat.SetType('seqaij');
mat.SetSizes(10,10,10,10);
%%
%  Create the ODE integrator
ts = PetscTS();
ts.SetProblemType(PetscTS.NONLINEAR);
ts.SetType('theta');
%%
%  Provide a function 
ts.SetFunction('odefunction',0);
type odefunction.m
%%
%  Provide a function that evaluates the Jacobian
ts.SetJacobian(mat,mat,'odejacobian',0);
type odejacobian.m
%%
%  Solve the ODE
ts.MonitorSet('tsmonitor');
ts.SetFromOptions();
ts.Solve(x);
x.View(viewer);
ts.View(viewer);
%%
%   Free PETSc objects and shutdown PETSc
%
x.Destroy();
mat.Destroy();
ts.Destroy();
viewer.Destroy();
PetscFinalize();