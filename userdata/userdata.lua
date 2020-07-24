
a = array.new(10)
print(a)
print('size :',array.size(a))
for i = 1, 1000 do
    array.set(a, i, 1 / i)
    print(array.get(a,i))
end


