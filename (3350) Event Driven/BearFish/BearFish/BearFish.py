import turtle
import random
import time

class World():
    def __init__(self, mx, my):
        self.maxX = mx
        self.maxY = my
        self.thingList = []
        self.grid = []

        for arow in range(self.maxY):
            row = []
            for acol in range(self.maxX):
                row.append(None)
            self.grid.append(row)

        self.wturtle = turtle.Turtle()
        self.wscreen = turtle.Screen()
        self.wscreen.setworldcoordinates(0,0,self.maxX-1, self.maxY-1)
        self.wscreen.addshape("Bear.gif")
        self.wscreen.addshape("Fish.gif")
        self.wturtle.hideturtle()

        self.wscreen.onkey(self.changeModeUp, "Right")
        self.wscreen.onkey(self.changeModeDown, "Left")
        self.wscreen.onkey(self.ModeUp, "Up")
        self.wscreen.onkey(self.ModeDown, "Down")
        self.wscreen.listen()

        self.inputmodes = {0: ['Simulation Speed Delay', 0], 1: ['Bear Breed Tick', 15], 2: ['Fish Breed Tick', 7], 3: ['Bear Starve Tick', 13]}
        self.selected_input_mode = 0

    def changeModeUp(self):
        self.selected_input_mode = self.selected_input_mode + 1
        self.updateDisplay()

    def changeModeDown(self):
        self.selected_input_mode = self.selected_input_mode - 1
        if self.selected_input_mode < 0:
            self.selected_input_mode = 0
        out = self.inputmodes[self.selected_input_mode % len(self.inputmodes)]
        self.updateDisplay()

    def ModeUp(self):
        self.inputmodes[self.selected_input_mode % len(self.inputmodes)][1] += 1
        self.updateDisplay()

    def ModeDown(self):
        self.inputmodes[self.selected_input_mode % len(self.inputmodes)][1] -= 1
        self.updateDisplay()

    def updateDisplay(self):
        out = self.inputmodes[self.selected_input_mode % len(self.inputmodes)]
        
        speed = self.wturtle.speed()                
        
        self.wturtle.penup()
        self.wturtle.goto(0, self.getMaxY() - 1.8)
        self.wturtle.pendown()
        self.wturtle.pencolor(0,.5,1)
        self.wturtle.width(14)
        self.wturtle.speed(0)
        self.wturtle.goto(7, self.getMaxY() - 1.8)
        self.wturtle.penup()

        self.wturtle.speed(speed)
       
        self.wturtle.goto(0, self.getMaxY() - 2)
        self.wturtle.pendown()
        self.wturtle.pencolor(0,0,0)
        self.wturtle.width(1)
        self.wturtle.write (out)
        self.wturtle.penup()

   #     self.wturtle.write(out)
        self.wscreen.tracer(1, self.inputmodes[0][1])
        print(out)

    def draw(self):
        self.updateDisplay()
        self.wscreen.tracer(0, self.inputmodes[0][1])
        self.wturtle.goto(0, 0)
        self.wturtle.pendown()
        self.wturtle.forward(self.maxX-1)
        self.wturtle.left(90)
        self.wturtle.forward(self.maxY-1)
        self.wturtle.left(90)
        self.wturtle.forward(self.maxX-1)
        self.wturtle.left(90)
        self.wturtle.forward(self.maxY-1)
        self.wturtle.left(90)

        for i in range(self.maxY-1):
            self.wturtle.forward(self.maxX-1)
            self.wturtle.backward(self.maxX-1)
            self.wturtle.left(90)
            self.wturtle.forward(1)
            self.wturtle.right(90)

        self.wturtle.forward(1)
        self.wturtle.right(90)

        for i in range(self.maxX-2):
            self.wturtle.forward(self.maxY-1)
            self.wturtle.backward(self.maxY-1)
            self.wturtle.left(90)
            self.wturtle.forward(1)
            self.wturtle.right(90)

        self.wscreen.tracer(1, self.inputmodes[0][1])

    def freezeWorld(self):
        self.wscreen.exitonclick()

    def addThing(self, athing, x, y):
        athing.setX(x)
        athing.setY(y)
        self.grid[y][x] = athing
        athing.setWorld(self)
        self.thingList.append(athing)
        athing.appear()

    def delThing(self, athing):
        athing.hide()
        self.grid[athing.getY()][athing.getX()] = None
        self.thingList.remove(athing)

    def moveThing(self, oldx, oldy, newx, newy):
        self.grid[newy][newx] = self.grid[oldy][oldx]
        self.grid[oldy][oldx] = None

    def getMaxX(self):
        return self.maxX

    def getMaxY(self):
        return self.maxY

    def liveALittle(self):
        if self.thingList != []:
            athing = random.randrange(len(self.thingList))
            randomthing = self.thingList[athing]
            randomthing.liveALittle()

    def emptyLocation(self,x,y):
        return self.grid[y][x] == None

    def lookAtLocation(self, x,y):
        return self.grid[y][x]
    
class Fish:
    def __init__(self):
        self.turtle = turtle.Turtle()
        self.turtle.up()
        self.turtle.hideturtle()
        self.turtle.shape("Fish.gif")

        self.xpos = 0
        self.ypos = 0
        self.world = None

        self.breedTick = 0

    def setX(self,newx):
        self.xpos = newx
    
    def setY(self,newy):
        self.ypos = newy

    def getX(self):
       return self.xpos

    def getY(self):
        return self.ypos
    
    def setWorld(self,aworld):
        self.world = aworld

    def appear(self):
        self.turtle.setpos(self.getX(), self.getY())
        self.turtle.showturtle()

    def hide(self):
        self.turtle.hideturtle()

    def tryToMove(self):
        offsetList = [(-1,1),(0,1),(1,1),(-1,0),(1,0),(-1,-1),(0,-1),(1,-1)]

        random.shuffle(offsetList)
                            
        for offset in offsetList:           
            newx = self.xpos + offset[0]             
            newy = self.ypos + offset[1]
            if 0 <= newx and newx < self.world.getMaxX() and 0 <= newy and newy < self.world.getMaxY():          
                if (self.world.emptyLocation(newx,newy)):
                    self.move(newx, newy)
                    break
            
    
    def move(self,newx,newy):
        self.world.moveThing(self.xpos,self.ypos,newx,newy)
        self.setX(newx)
        self.setY(newy)
        self.appear()

    def tryToBreed(self):
        self.breedTick = 0

        offsetList = [(-1,1),(0,1),(1,1),(-1,0),(1,0),(-1,-1),(0,-1),(1,-1)]
        random.shuffle(offsetList)
                            
        for offset in offsetList:                    
            newx = self.xpos + offset[0]             
            newy = self.ypos + offset[1]
            if 0 <= newx and newx < self.world.getMaxX() and 0 <= newy and newy < self.world.getMaxY():          
                if (self.world.emptyLocation(newx,newy)):
                    newfish = Fish()
                    self.world.addThing(newfish, newx, newy)
                    break

    def liveALittle(self):
       offsetList = [(-1,1),(0,1),(1,1),(-1,0),(1,0),(-1,-1),(0,-1),(1,-1)]
       adjfish = 0                            
       for offset in offsetList:                    
           newx = self.xpos + offset[0]             
           newy = self.ypos + offset[1]
           if 0 <= newx and newx < self.world.getMaxX() and 0 <= newy and newy < self.world.getMaxY():          
               if (not self.world.emptyLocation(newx,newy)) and isinstance(self.world.lookAtLocation(newx,newy),Fish):
                   adjfish = adjfish + 1
       
       m = 10  
       rn = random.randrange(1, m + 1)
        
       if rn > m / 4 and adjfish >= 3:                 
           self.world.delThing(self)
       elif adjfish >= 2:
           self.world.delThing(self)    
       else:
           self.breedTick = self.breedTick + 1
           if self.breedTick >= self.world.inputmodes[2][1]:
               self.tryToBreed()
           self.tryToMove()

class Bear:
    def __init__(self):
        self.turtle = turtle.Turtle()
        self.turtle.up()
        self.turtle.hideturtle()
        self.turtle.shape("Bear.gif")

        self.xpos = 0
        self.ypos = 0
        self.world = None

        self.breedTick = 0
        self.starveTick = 0
        self.energyLevel = 10

    def setX(self,newx):
        self.xpos = newx
    
    def setY(self,newy):
        self.ypos = newy

    def getX(self):
       return self.xpos

    def getY(self):
        return self.ypos
    
    def setWorld(self,aworld):
        self.world = aworld

    def appear(self):
        self.turtle.setpos(self.getX(), self.getY())
        self.turtle.showturtle()

    def hide(self):
        self.turtle.hideturtle()

    def tryToMove(self):
        offsetList = [(-1,1),(0,1),(1,1),(-1,0),(1,0),(-1,-1),(0,-1),(1,-1)]

        random.shuffle(offsetList)
                            
        for offset in offsetList:           
            newx = self.xpos + offset[0]             
            newy = self.ypos + offset[1]
            if 0 <= newx and newx < self.world.getMaxX() and 0 <= newy and newy < self.world.getMaxY():          
                if (self.world.emptyLocation(newx,newy)):
                    self.move(newx, newy)
                    break
            
    
    def move(self,newx,newy):
        self.world.moveThing(self.xpos,self.ypos,newx,newy)
        self.energyLevel -= 1
        self.setX(newx)
        self.setY(newy)
        self.appear()

    def tryToBreed(self):
        self.breedTick = 0

        offsetList = [(-1,1),(0,1),(1,1),(-1,0),(1,0),(-1,-1),(0,-1),(1,-1)]
        random.shuffle(offsetList)
                            
        for offset in offsetList:                    
            newx = self.xpos + offset[0]             
            newy = self.ypos + offset[1]
            if 0 <= newx and newx < self.world.getMaxX() and 0 <= newy and newy < self.world.getMaxY():          
                if (self.world.emptyLocation(newx,newy)):
                    newbear = Bear()
                    self.world.addThing(newbear, newx, newy)
                    self.energyLevel -= 1
                    break

    def tryToEat(self):
        if True or self.energyLevel <= self.world.inputmodes[3][1] * 3:#only eat when hungry
            offsetList = [(-1,1),(0,1),(1,1),(-1,0),(1,0),(-1,-1),(0,-1),(1,-1)]

            random.shuffle(offsetList)#removes bias
                            
            for offset in offsetList:           
                newx = self.xpos + offset[0]             
                newy = self.ypos + offset[1]
                if 0 <= newx and newx < self.world.getMaxX() and 0 <= newy and newy < self.world.getMaxY():          
                    if (not self.world.emptyLocation(newx,newy) and isinstance(self.world.lookAtLocation(newx, newy), Fish)):
                        self.starveTick = 0
                        self.energyLevel += 7
                        self.world.delThing(self.world.lookAtLocation(newx, newy))
                        self.move(newx, newy)
                        break

        self.tryToMove()

    def liveALittle(self):
       self.starveTick = self.starveTick + 1
       self.tryToEat()
       if self.energyLevel <= 0:
           self.world.delThing(self)
       else:
           self.breedTick = self.breedTick + 1
           if self.breedTick >= self.world.inputmodes[1][1]:
               self.tryToBreed()
               
def mainSimulation():
    numberOfBears = 5
    numberOfFish = 20
    worldLifeTime = 10000
    worldWidth = 30
    worldHeight = 30
    
    myworld = World(worldWidth,worldHeight)      
    myworld.draw()                           

    for i in range(numberOfFish): 
        newfish = Fish()
        x = random.randrange(myworld.getMaxX())
        y = random.randrange(myworld.getMaxY())
        while not myworld.emptyLocation(x,y):
            x = random.randrange(myworld.getMaxX())
            y = random.randrange(myworld.getMaxY())
        myworld.addThing(newfish,x,y)        
    
    for i in range(numberOfBears):   
        newbear = Bear()
        x = random.randrange(myworld.getMaxX())
        y = random.randrange(myworld.getMaxY())
        while not myworld.emptyLocation(x,y):   
            x = random.randrange(myworld.getMaxX())
            y = random.randrange(myworld.getMaxY())
        myworld.addThing(newbear,x,y)     
    
    f = open('world.csv', 'w')
    f.truncate()
    f.write('timestamp, bears, fish\n')
    for i in range(worldLifeTime):
        myworld.liveALittle()

        bearCount = 0
        fishCount = 0

        for i in myworld.thingList:            
            if isinstance(i, Bear):
                bearCount = bearCount + 1
            elif isinstance(i, Fish):
                fishCount = fishCount + 1

        f.write(time.ctime() + ',' + str(bearCount) + ',' + str(fishCount) + '\n')
        if bearCount == 0 or fishCount == 0:
            break
    f.close()
    
    myworld.freezeWorld()  


mainSimulation()
