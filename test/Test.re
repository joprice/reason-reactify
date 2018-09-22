/** Simple test cases */

open Reactify;

module TestReconciler = {
    type recordType = {
        testVal: int
    };

    type primitives = 
        | Root
        | A(int)
        | B
        | C
        | D(recordType);

    type node = {
        children: ref(list(node)),
        nodeType: primitives,
    };

    let createInstance = (prim) => {
        print_endline ("create instance called");
        {
            children: ref([]),
            nodeType: prim
        };
    };

    let appendChild = (parent, child) => {
        parent.children := parent.children^ @ [child];
        print_endline("append child - new count: " ++ string_of_int(List.length(parent.children^)));
    };

    let removeChild = (parent, child) => {
        print_endline("remove child");
    };

    let updateInstance = (n, prim) => {
        print_endline ("update element");
    };

    let a11 = A(1);
    let a12 = A(1);
    let a2 = A(2);

    let d11 = D({testVal: 1});
    let d12 = D({testVal: 1});
    let d2 = D({testVal: 2});

let equal_constructors = (x: 'a, y: 'a) => {
  let r = Obj.repr(x)
  and s = Obj.repr(y);
  if (Obj.is_int(r) && Obj.is_int(s)) {
    (Obj.obj(r): int) == (Obj.obj(s): int);
  } else if (Obj.is_block(r) && Obj.is_block(s)) {
    Obj.tag(r) == Obj.tag(s);
  } else {
    false;
  };
};

    print_endline ("a11 == a12? " ++ string_of_bool(a11 == a12));
    print_endline ("a12 == a2?" ++ string_of_bool(a2 == a11));
    print_endline("equal constructors a11 & a2?" ++ string_of_bool(equal_constructors(a11, a2)));
    print_endline("equal constructors b & c?" ++ string_of_bool(equal_constructors(B, C)));

    print_endline ("d11 == d12? " ++ string_of_bool(d11 == d12));
    print_endline ("d12 == d2?" ++ string_of_bool(d2 == d11));
    print_endline ("equal constructors d & d?" ++ string_of_bool(equal_constructors(d11, d2)));
}

module TestReact = Reactify.Make(TestReconciler);

let rootNode: TestReconciler.node = {
    children: ref([]),
    nodeType: Root
};

let aComponent = (~testVal, ~children, ()) => TestReact.primitiveComponent(A(testVal), children);
let bComponent = (~children, ()) => TestReact.primitiveComponent(B, children);
let cComponent = (~children, ()) => TestReact.primitiveComponent(C, children);

let component = () => {
    <aComponent testVal={1}>
        <bComponent />
        <bComponent>
            <cComponent />
        </bComponent>
    </aComponent>
}

TestReact.render(rootNode, component());

open TestReconciler;

type tree('a) =
    | TreeNode('a, list(tree('a)))
    | TreeLeaf('a);

let expectedStructure: tree(primitives) = TreeNode(Root, 
                             [TreeNode(
        A(1), [
         TreeLeaf(B),
         TreeNode(B, [TreeLeaf(C)])
        ])
                             ]
);

let validateStructure = (rootNode: node , structure: tree(primitives)) => {
    let rec f = (inputNode: node, st: tree(primitives), level) => {
        switch (st) {
        | TreeNode(p, c) => {
            assert(inputNode.nodeType == p);
            assert(List.length(inputNode.children^) == List.length(c));

            List.iter2((a, b) => f(a, b, level + 1), inputNode.children^, c)
        }
        | TreeLeaf(p) => {
            assert(inputNode.nodeType == p);
        };
    };
    };

    f(rootNode, structure, 0);
};

validateStructure(rootNode, expectedStructure);

TestReact.render(rootNode, component());

validateStructure(rootNode, expectedStructure);
