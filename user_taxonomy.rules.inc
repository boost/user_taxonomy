<?php
/**
* Implementation of hook_rules_condition_info().
* @ingroup rules
*/
function user_taxonomy_rules_condition_info() {
  return array(
    'user_taxonomy_condition_taxonomy_term' => array(
      'label' => t('User has taxonomy term'),
      'arguments' => array(
        'user' => array('type' => 'user', 'label' => t('User')),
        'term' => array('type' => 'number', 'label' => t('Taxonomy term tid'))
      ),
      'module' => 'User'
    ),
    'user_taxonomy_condition_node_taxonomy' => array(
      'label' => t('User has any taxonomy term from content'),
      'arguments' => array(
        'user' => array('type' => 'user', 'label' => t('User')),
        'node' => array('type' => 'node', 'label' => t('Content'))
      ),
      'module' => 'User'      
    )
  );
}

/**
* Implementation of hook_rules_action_info().
* @ingroup rules
*/
function user_taxonomy_rules_action_info() {
  return array(
    'user_taxonomy_action_set_node_taxonomy' => array(
      'label' => t('Add the users taxonomy terms to a content node'),
      'arguments' => array(
        'user' => array('type' => 'user', 'label' => t('User from which to get terms')),
        'node' => array('type' => 'node', 'label' => t('Content')),
      ),
      'module' => 'Node',
    ),
  );
}

/**
* Rule condition that checks if a user has a taxonomy term
*/
function user_taxonomy_condition_taxonomy_term($user, $tid) {
  $terms = user_taxonomy_user_taxonomies($user);
  $map_tids = create_function('$term', 'return $term->tid;');
  $tids = array_unique(array_map($map_tids, $terms));
  return in_array($tid, $tids);
}

/**
* Rule condition that checks if a user has any taxonomy terms from a node
*/
function user_taxonomy_condition_node_taxonomy($user, $node) {
  $map_tids = create_function('$term', 'return $term->tid;');
  $user_terms = user_taxonomy_user_taxonomies($user);
  $user_tids = array_unique(array_map($map_tids, $user_terms));
  
  $node_terms = $node->taxonomy;
  $node_tids = array_unique(array_map($map_tids, $node_terms));
  
  $tids = array_intersect($user_tids, $node_tids);
  
  return(count($tids) > 0);
}

/**
* Apply all user taxonomy terms to a node
*/
function user_taxonomy_action_set_node_taxonomy($user, $node) {
  $user_terms = user_taxonomy_user_taxonomies($user);
  $node_terms = $node->taxonomy;
  
  foreach($user_terms as $user_term) {
    $add = TRUE;
    
    foreach($node_terms as $node_vid) {
      foreach($node_vid as $node_term_key => $node_term_value) {
      
        if($node_term_value == $user_term->tid) {
          $add = FALSE;
          break;
        }
      }
    }
    
    if($add) {
      $node->taxonomy[$user_term->vid][$user_term->tid] = $user_term->tid;
    }
  }
    
  return array('node' => $node);
}