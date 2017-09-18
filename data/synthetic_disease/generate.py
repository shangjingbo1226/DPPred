import random

random.seed(19910724)

N = 100000

out = open('train.csv', 'w')

out.write('age,gender=Male,gender=Female,LT1=A,LT1=B,LT1=O,LT1=AB,LT2,label\n')
for i in xrange(N):
    feats = []
    age = random.randint(1, 60)
    feats.append(str(age))
    
    gender = random.randint(0, 1)
    feats.extend([str(gender), str(1 - gender)])
    
    blood = random.randint(0, 3)
    feats.extend([str(int(blood == 0)), str(int(blood == 1)), str(int(blood == 2)), str(int(blood == 3))])
    
    LT2 = random.random()
    feats.extend([str(LT2)])
    
    label = 0
    if (age > 18) and (gender == 1) and (blood == 3) and (LT2 > 0.6):
        label = 1
    if (age > 18) and (gender == 0) and (blood == 2) and (LT2 > 0.5):
        label = 1
    if (age  <= 18) and (LT2 > 0.9):
        label = 1
        
    if random.random() < 0.001:
        label = 1 - label
    
    feats.append(str(label))
    
    out.write(','.join(feats) + '\n')
out.close()
    
out = open('test.csv', 'w')

out.write('age,gender=Male,gender=Female,LT1=A,LT1=B,LT1=O,LT1=AB,LT2,label\n')
for i in xrange(N/2):
    feats = []
    age = random.randint(1, 60)
    feats.append(str(age))
    
    gender = random.randint(0, 1)
    feats.extend([str(gender), str(1 - gender)])
    
    blood = random.randint(0, 3)
    feats.extend([str(int(blood == 0)), str(int(blood == 1)), str(int(blood == 2)), str(int(blood == 3))])
    
    LT2 = random.random()
    feats.extend([str(LT2)])
    
    label = 0
    if (age > 18) and (gender == 1) and (blood == 3) and (LT2 > 0.6):
        label = 1
    if (age > 18) and (gender == 0) and (blood == 2) and (LT2 > 0.5):
        label = 1
    if (age  <= 18) and (LT2 > 0.9):
        label = 1
    
    feats.append(str(label))
    
    out.write(','.join(feats) + '\n')
out.close()