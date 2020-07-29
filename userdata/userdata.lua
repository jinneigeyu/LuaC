a = array.new(1000)
for i = 1, 1000 do
    array.set(a, i, i) 
end
array.set(a, 10,0)
print(array.get(a, 10)) 
print(array.get(a, 11)) 
print(array.size(a)) 


