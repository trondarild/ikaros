%
% Multiplies two column vectors, where size of A is bigger and an integer 
% multiple of B
%

function retVal = spreadmult(A, B)
  dimA = size(A,1);
  dimB = size(B,1);

  sliceLength = dimA/dimB;
  retVal = zeros(dimA,1);
  currentPos = 1;
  i=1;
  for i=1:dimB
     retVal(currentPos:currentPos+sliceLength-1,1) = A(currentPos:currentPos+sliceLength-1,1)*B(i);   
     currentPos += sliceLength;
  end
end