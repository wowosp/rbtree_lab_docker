#include "rbtree.h"

#include <stdlib.h>

void delete_nodes(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *p);
void right_rotate(rbtree *t, node_t *p);
void rbtree_insert_fixup(rbtree *t, node_t *p);
void rbtree_delete_fixup(rbtree *t, node_t *p);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
node_t *successor(rbtree *t, node_t *s);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  node_t *nil = (node_t *)calloc(1,sizeof(node_t));
  nil->color = RBTREE_BLACK;
  nil->left = nil;
  nil->right = nil;
  nil->parent = nil;

  p->nil = nil;
  p->root = nil;

  return p;
}

void delete_nodes(rbtree *t, node_t *node) {
  if (node == t->nil) return; 
  
  delete_nodes(t, node->left);
  delete_nodes(t, node->right);

  free(node);
}

void delete_rbtree(rbtree *t) {
  if (t->root == t->nil) return;
  delete_nodes(t, t->root);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *cur = t->root;
  node_t *cur_parent = t->nil;
  node_t *new_node = (node_t *)malloc(sizeof(node_t));

  new_node-> key = key;

  while (cur != t->nil){
    cur_parent = cur;
    if (key < cur->key){
      cur=cur->left;
    } 
    else{
      cur=cur->right;
    }
  }

  new_node->parent=cur_parent;
  if (cur_parent == t->nil){
    t->root = new_node;
  }
  else if (new_node->key < cur_parent->key){
    cur_parent->left = new_node;
  }
  else{
    cur_parent->right = new_node;
  }

  new_node->left=t->nil;
  new_node->right=t->nil;
  new_node->color=RBTREE_RED;

  rbtree_insert_fixup(t, new_node);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;

  while (cur != t->nil){
    if (key == cur->key) {
      return cur; 
    } 
    else if (key < cur->key) {
      cur = cur->left;
    } 
    else {
      cur = cur->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *cur = t->root;

  while (cur->left != t->nil){
    cur = cur->left;
  }
  
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *cur = t->root;

  while (cur->right != t->nil){
    cur = cur->right;
  }
  
  return cur;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *cur = p;
  color_t cur_color = p->color;
  node_t *delete_location;

  if (p->left == t->nil){
    delete_location = p->right;
    rbtree_transplant(t,p,p->right);
  }

  else if (p->right == t->nil){
    delete_location = p->left;
    rbtree_transplant(t,p,p->left);
  }

  else{
    cur = successor(t,p->right);
    cur_color = cur->color;
    delete_location = cur->right;

    if (cur != p->right){
      rbtree_transplant(t,cur,cur->right);
      cur->right=p->right;
      cur->right->parent=cur;
    }
    else{
      delete_location->parent=cur;
    }
    rbtree_transplant(t,p,cur);
    cur->left=p->left;
    cur->left->parent=cur;
    cur->color=p->color;
  }

  if (cur_color == RBTREE_BLACK){
    rbtree_delete_fixup(t, delete_location);
  }

  free(p);
  
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *stack_arr[n];
  node_t *cur = t->root;
  int i = 0;

  while (cur != t->nil) {
    stack_arr[i] = cur;
    i++;
    cur = cur->left;
  }

  int j = 0;
  while (i > 0 && j < n) {
    i--;
    node_t *temp = stack_arr[i];
    arr[j] = temp->key;
    j++;

    if (temp->right != t->nil) {
      cur = temp->right;
      while (cur != t->nil) {
        stack_arr[i] = cur;
        i++;
        cur = cur->left;
      }
    }
  }

  return 0;
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right;
  x->right= y->left;

  if ((y->left) != (t->nil)){
    y->left->parent=x;
  }

  y->parent=x->parent;

  if ((x->parent)==(t->nil)){
    t->root=y;
  }

  else if (x==(x->parent->left)){
    x->parent->left = y;
  }
  
  else{
    x->parent->right=y;
  }

  y->left=x;
  x->parent=y;
}

void right_rotate(rbtree *t, node_t *x){
  node_t *y = x->left;
  x->left= y->right;

  if ((y->left) != (t->nil)){
    y->right->parent=x;
  }

  y->parent=x->parent;

  if ((x->parent)==(t->nil)){
    t->root=y;
  }

  else if (x==(x->parent->right)){
    x->parent->right = y;
  }
  
  else{
    x->parent->left = y;
  }

  y->right=x;
  x->parent=y;
}

void rbtree_insert_fixup(rbtree *t, node_t *check_node){
  node_t *uncle;

  while (check_node->parent->color == RBTREE_RED){

    if (check_node->parent == check_node->parent->parent->left){
      uncle = check_node->parent->parent->right;

      if (uncle->color == RBTREE_RED){
        check_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        check_node->parent->parent->color = RBTREE_RED;
        check_node = check_node->parent->parent;
      }

      else{
        if(check_node==check_node->parent->right){
          check_node=check_node->parent;
          left_rotate(t,check_node);
        }

        check_node->parent->color = RBTREE_BLACK;
        check_node->parent->parent->color = RBTREE_RED;
        right_rotate(t,check_node->parent->parent);
      }
    }
    else{
      uncle = check_node->parent->parent->left;

      if (uncle->color == RBTREE_RED){
        check_node->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        check_node->parent->parent->color = RBTREE_RED;
        check_node = check_node->parent->parent;
      }

      else{
        if(check_node==check_node->parent->left){
          check_node=check_node->parent;
          right_rotate(t,check_node);
        }

        check_node->parent->color = RBTREE_BLACK;
        check_node->parent->parent->color = RBTREE_RED;
        left_rotate(t,check_node->parent->parent);
      }
    }
  }
  t->root->color=RBTREE_BLACK;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil){
    t->root=v;
  }
  else if (u == u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right=v;
  }
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *p){
  node_t *w;

  while(p != t->root && p->color == RBTREE_BLACK){
    if (p == p->parent->left){
      w=p->parent->right;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        left_rotate(t,p->parent);
        w=p->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        p=p->parent;
      }
      else{
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t,w);
          w = p->parent->right;
        }
        w->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, p->parent);
        p = t->root;
      }
    }
    else{
      w=p->parent->left;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        right_rotate(t,p->parent);
        w=p->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        p=p->parent;
      }
      else{
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t,w);
          w = p->parent->left;
        }
        w->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, p->parent);
        p = t->root;
      }
    }
  }
  p->color=RBTREE_BLACK;
}

node_t *successor(rbtree *t, node_t *s){
  node_t *cur = s;

  while (cur->left != t->nil){
    cur = cur->left;
  }
  
  return cur;
}