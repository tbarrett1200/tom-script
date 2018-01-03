# Technical Document
See more detail at
https://github.com/tbarrett1200 !

## Assembly Language Programming
Assembly language, also known as machine language, is the representation of the
instructions that the processor is actually running. Since it is so low level,
programming in Assembly is not practical. However, learning to do so was a
great mental exercise and also helped me understand higher level languages such
as a more foundational level. In my journey to create my own programming
language with a handwritten compiler, this was very important. All compiled
languages are at some point converted to low level Assembly code.

### Hello World
The classic first program when learning any language is Hello World. This
program prints out "Hello World" to the console.
```
.section __DATA,__data

str1:
  .asciz "Hello World\n"

.section __TEXT,__text

.globl start

start:
  andq $0xfffffffffffffff0, %rsp

  leaq str1(%rip), %rdi
  movq $0, %rax
  callq _printf

  movq $0, %rdi
  movq $0x2000001, %rax
  syscall
```
### Command Line Arguments
Assembly programs do not have run time environments that are as convenient as
high level languages, however it is possible to access environment data such as
program arguments and environment variables. This program prints all arguments
back to the console.
```
.section __DATA,__data

str1:
    .asciz "argc: %d\n"
str2:
    .asciz "arg: %s\n"

.section __TEXT,__text

.globl start

start:

  movq (%rsp), %r12
  leaq 8(%rsp), %r13

  movq $0, %rdx
  movq %rsp, %rax
  movq $16, %rcx
  divq %rcx

  movq %rdx, %r14
  subq %r14, %rsp

  leaq str1(%rip), %rdi
  movq %r12, %rsi
  movq $0, %rax
  callq _printf

  movq $0, %r14

loop:
  leaq str2(%rip), %rdi
  movq (%r13, %r14, 8), %rsi
  movq $0, %rax
  callq _printf

  incq %r14

  cmp %r12, %r14
  jne loop

	movq $0, %rdi
  movq $0x2000001, %rax
  syscall
```

### Flow Control: While Loop
Flow control works different in assembly, and is a lot more freeform. I created
the equivalent of a while loop which prints the numbers from 1 to 100 to the
console.
```
.section __DATA,__data

str1:
  .asciz "value: %d\n"

.section __TEXT,__text

.globl start

start:
  andq $0xfffffffffffffff0, %rsp
  movq $100, %rbx

compare:
  cmpq $0, %rbx
  je end

  leaq str1(%rip), %rdi
  movq $0, %rax
  movq %rbx, %rsi
  callq _printf
  decq %rbx

  jmp compare
end:
  movq $0, %rdi
  movq $0x2000001, %rax
  syscall
```

## Data Structure Implementation: Linked List, Hash Map
Data structures are a very important part of computer science. Nearly all
complex programs require some sort of advanced data structures. Although there
are generic libraries with similar functionality available, creating my own
allowed me to broaden my understanding of computer science as a whole. It also
gave me practice programming in C. I used this library for use in some
of my other projects including my compiler for my computer language. The
implementations can be found on my github page.
```c
#ifndef LIST_H
#define LIST_H

typedef struct Node
{
    void *data;
    struct Node *next;
} Node;

typedef struct List
{
    int size;
    struct Node *head;
} List;

list list_init();
void list_add(List *l, void *data);
int list_size(List l);
void* list_get(List l, int i);
void list_clear(List *l,void (*destroy_data)(void *));
void list_print(List l, void (*print_data)(void*));

#endif /* linked_list.h */
```
```c
#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
    int size;
    int capacity;
    void **data;
} Vector;

Vector* vector_init(void);
void vector_add(Vector *v, void *data);
int vector_size(Vector *v);
void* vector_get(Vector *l, int i);
void vector_destroy(Vector *v, void *destroy);

#endif /* vector.h */
```
```c
#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "vector.h"

struct Hash;

typedef struct
{
    void *key;
    void *value;
} HashNode;

typedef struct Hash
{
    int size;
    Vector **data;
    int (*cmp)(void*, void*);
    int (*hash)(void*);
} Hash;

Hash* hash_init(void *hash, void *compare);
void hash_set(Hash*, void *key, void *value);
void* hash_get(Hash*, void *key);
void hash_destroy(Hash *h, void *destroyKey, void *desroyValue);

#endif /* hash_map.h */
```
## Compiler Design: Creating a Programming Language

Input:
```
var a
var b
var c

function doSomething {
    return a + b * c
}

function main {
    a = 2
    b = 3
    c = 5
    return 2 * doSomething
}
```

Output:
```
target triple = "x86_64-apple-macosx10.12.0"
@a = global i32 0
@b = global i32 0
@c = global i32 0

define i32 @doSomething () {
entry:
	%0 = load i32, i32* @a
	%1 = load i32, i32* @b
	%2 = load i32, i32* @c
	%3 = mul i32 %1, %2
	%4 = add i32 %0, %3
	ret i32 %4
}

define i32 @main () {
entry:
	store i32 2, i32* @a
	store i32 3, i32* @b
	store i32 5, i32* @c
	%0 = call i32 @doSomething()
	%1 = mul i32 2, %0
	ret i32 %1
}
```

# Javascript Parsing Library: Building Interpreters
A Node.js library which allows for parsing arbitrary LL(1) grammars. Shown below is a simple example of an implementation of a parser for numeric expressions which follow the order of operations using my library.

The library can also be used to interpret syntax trees generated by the grammar. The complete implementation for the expression interpreter can be found on my github page: https://github.com/tbarrett1200. Complete programming languages can be created using the library, but it is best used for smaller data structure parsing schemes.

```js
const Parser = require('./parser');
const lexer = require('./c-lexer')
/**
 * A reference expression parser
 */
module.exports = new Parser(function(g) {
  g.start = 'expression';
  g.lexer = lexer;

  g.symbol('expression')
  .rule('term', 'expression-list');

  g.symbol('expression-list')
  .rule('term-operator', 'term', 'expression-list')
  .optional();

  g.symbol('term')
  .rule('factor', 'term-list')

  g.symbol('term-list')
  .rule('factor-operator', 'factor', 'term-list')
  .optional()

  g.symbol('factor')
  .rule('number')
  .rule('(', 'expression', ')');
});
```

# Deep Learning: Neural Networks
A proof-of-concept Python program which builds Neural Networks of arbitrary size for multidimensional function regression using the NumPy linear algebra library. This program can be modified to work with classification problems as well.
``` Python
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import math
import functions as f

np.random.seed(1)

def initialize(dims):
    layers = []
    for l in range(1,len(dims)):
        layers.append({
            'W': np.random.randn(dims[l], dims[l-1]) * np.sqrt(2/dims[l-1]),
            'b': np.zeros((dims[l], 1)),
        })
    return layers

# (n(l), m)
def forward(layers, x):
    for l in layers:
        # stores input activations for backpropogation
        l['x'] = x
        l['z'] = l['W'].dot(x) + l['b']
        x = l['func'](l['z'])
        l['a'] = x
    return x

def backward(layers, m, dA):
    for l in reversed(range(len(layers))):
        dZ = dA * layers[l]['func'](layers[l]['z'], True)
        dW = 1/m * np.dot(dZ,layers[l]['x'].T)
        db = 1/m * np.sum(dZ, axis=1, keepdims=True)
        dA = np.dot(layers[l]['W'].T, dZ)

        layers[l]['dW'] = dW
        layers[l]['db'] = db

def train(size, alpha, epochs, m, x, y, print_rate):
    # omitted to reduce size, can be found online

def cost(layers, m, x, y):
    a = forward(layers, x)
    return np.squeeze(1/(2*m) * np.sum((a - y) ** 2, axis=1, keepdims=True))

# ommited for brevity
```

## Double Helix iOS Game
For an explanation of gameplay and intent, refer to the overview pdf document. Complete source code can be found at https://github.com/tbarrett1200.

Supporting Enum Source Code;
```Swift
import Foundation
import UIKit

/**
 A nitrogenous base that composes DNA
 */
enum Nucleobase {
    case adenine, thymine, cytosine, guanine

    /**
     Creates a new nuclebase of a random type
     */
    init() {
        let random = Int(arc4random_uniform(4))
        switch random {
        case 0: self = .adenine
        case 1: self = .thymine
        case 2: self = .guanine
        default: self = .cytosine
        }
    }

    /**
     The representative color of the nucleobase. Matching bases have similar hues
     */
    var color: UIColor {
        switch self {
        case .adenine: return ColorTheme.green
        case .thymine: return ColorTheme.blue
        case .guanine: return ColorTheme.yellow
        case .cytosine: return ColorTheme.red
        }
    }

    /**
     Returns the complementary base type
     */
    var complement: Nucleobase {
        switch self {
        case .adenine: return .thymine
        case .thymine: return .adenine
        case .guanine: return .cytosine
        case .cytosine: return .guanine
        }
    }

    /**
     Returns whether or not the nucleobase can pair with the other specified base
     */
    func canPair(with base: Nucleobase?) -> Bool {
        print("\(self) \(base)")
        return self.complement == base
    }
}
```

Main Gameplay Source Code:
```
import Foundation
import SpriteKit
import GameKit

class GameScene: SKScene {
    private let maxMutations = 5

    public static var score = 0
    public static var highScore: Int = 0

    private var dna: DNA!

    private var scoreLabel: SKLabelNode! = SKLabelNode(fontNamed: "Avenir")
    private var mutationLabel: SKLabelNode! = SKLabelNode(fontNamed: "Avenir")

    private var adenine: SKShapeNode! = SKShapeNode(circleOfRadius: 40)
    private var thymine: SKShapeNode! = SKShapeNode(circleOfRadius: 40)
    private var cytosine: SKShapeNode! = SKShapeNode(circleOfRadius: 40)
    private var guanine: SKShapeNode! = SKShapeNode(circleOfRadius: 40)

    override func didMove(to view: SKView) {


        backgroundColor = ColorTheme.dark

        GameScene.score = 0
        GameScene.highScore = UserDefaults.standard.integer(forKey: "highScore")

        reportScoreToLeaderboard()

        adenine.fillColor = Nucleobase.adenine.color
        adenine.position = CGPoint(x: 60, y: 60)

        thymine.fillColor = Nucleobase.thymine.color
        thymine.position = CGPoint(x: 160, y: 60)

        cytosine.fillColor = Nucleobase.cytosine.color
        cytosine.position = CGPoint(x: frame.width - 160, y: 60)

        guanine.fillColor = Nucleobase.guanine.color
        guanine.position = CGPoint(x: frame.width - 60, y: 60)

        for b in [adenine!, thymine!, cytosine!, guanine!] {
            b.strokeColor = b.fillColor
            b.isAntialiased = true
            addChild(b)
        }
        scoreLabel.fontSize = 24
        scoreLabel.text = "SCORE: 0"
        scoreLabel.verticalAlignmentMode = .top
        scoreLabel.position = CGPoint(x: frame.width/2, y: adenine.frame.maxY - 10)

        mutationLabel.fontSize = 18
        mutationLabel.text = "MUTATIONS: 0"
        mutationLabel.verticalAlignmentMode = .bottom
        mutationLabel.position = CGPoint(x: frame.width/2, y: adenine.frame.minY + 10)

        for l in [scoreLabel!, mutationLabel!] {
            l.color = ColorTheme.light
            l.horizontalAlignmentMode = .center
            addChild(l)
        }

        dna = DNA(size: CGSize(width:frame.size.width, height: frame.size.height/2))
        dna.position = CGPoint(x: frame.width, y: 0.5 * (frame.height - adenine.frame.maxY) + adenine.frame.maxY)
        addChild(dna)
    }



    func didLoseGame() {
        if GameScene.score > GameScene.highScore {
            GameScene.highScore = GameScene.score
            UserDefaults.standard.set(GameScene.highScore, forKey: "highScore")
        }

        reportScoreToLeaderboard()

        if let view = view {
            let scene = GameOverScene(size: view.bounds.size)
            scene.scaleMode = .aspectFit
            view.ignoresSiblingOrder = true
            view.presentScene(scene, transition: .fade(withDuration: 1))
        }
    }

    func reportScoreToLeaderboard() {
        if GKLocalPlayer.localPlayer().isAuthenticated {
            let score = GKScore(leaderboardIdentifier: "tbarrett1200.doubleHelix.highScore", player: GKLocalPlayer.localPlayer())
            score.value = Int64(GameScene.highScore)
            GKScore.report([score], withCompletionHandler: nil)
        }
    }

    func mutationsChanged() {
        mutationLabel.text = "MUTATIONS: \(self.dna.mutations)"
        if dna.mutations == dna.maxMutations {
            didLoseGame()
        }
    }

    func scoreChanged() {
        scoreLabel.text = "SCORE: \(GameScene.score)"
    }
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {

        var base: Nucleobase? = nil

        if let touch = touches.first {
            if adenine.frame.contains(touch.location(in: self)) {
                base = .adenine
            } else if thymine.frame.contains(touch.location(in: self)) {
                base = .thymine
            } else if cytosine.frame.contains(touch.location(in: self)) {
                base = .cytosine
            } else if guanine.frame.contains(touch.location(in: self)) {
                base = .guanine
            }
        }

        if let base = base {
            dna.addNucleotide(with: base)
        }
    }
```
