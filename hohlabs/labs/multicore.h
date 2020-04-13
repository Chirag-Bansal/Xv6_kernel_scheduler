#pragma once
#include <atomic>
//
// INVARIANT: w_deleted_count <= w_deleting_count <= w_cached_read_count <= shared_read_count <= r_reading_count <= r_cached_write_count <= shared_write_count <= w_writing_count <= w_deleted_count + MAX_SIZE
//
// INVARIANT:      w_writing_count      - w_deleted_count     <= MAX_SIZE
// =========>      w_writing_count      - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - shared_read_count   <= MAX_SIZE
//
//
// INVARIANT: 0 <= r_cached_write_count - r_reading_count
// =========> 0 <= r_cached_write_count - shared_read_count
// =========> 0 <= shared_write_count   - shared_read_count
//
//
// THEOREM: =========> 0 <= shared_write_count   - shared_read_count   <= MAX_SIZE
//





//
//
// Channel/Queue:
//
// Shared between Producer and Consumer
//
struct channel_local{
  public:

    //insert your code here
    int w_head;
    int r_head;
    int d_head;

    // int w_round;
    // int r_round;
    // int d_round;

  public:

    //
    // Intialize
    //
    channel_local(){

      // insert your code here
      w_head = 0;
      r_head = 0;
      d_head = 0;

      // w_round = -1;
      // r_round = -1;
      // d_round = -1;

    }
};

struct channel_t{
  public:

    //insert your code here
    std::atomic<int>  w_pnt;
    std::atomic<int>  r_pnt;

  public:

    //
    // Intialize
    //
    channel_t(){

      // insert your code here
      w_pnt = 0;
      r_pnt = 0;
    }
};



//
// Producer's (not shared)
//
struct writeport_t{
public:
    //insert your code here
    channel_local write_channel;
    // int read_counter = -1;
    size_t buffer_size;
public:

  //
  // Intialize
  //
  writeport_t(size_t tsize)
  {
    //insert code here
    buffer_size = tsize;

  }

public:

  //
  // no of entries available to write
  //
  // helper function for write_canreserve
  //
  size_t write_reservesize(){

    // insert your code here
    if(write_channel.w_head - write_channel.d_head>=0 && buffer_size>=write_channel.w_head - write_channel.d_head){
      return (buffer_size-write_channel.w_head+write_channel.d_head);
    }
    else{
      return 0;
    }
  }

  //
  // Can write 'n' entries?
  //
  bool write_canreserve(size_t n){

    // insert your code here
    if(write_reservesize()>=n){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entries for write
  //
  size_t write_reserve(size_t n){
    // insert your code here
    // hoh_debug("While writing => w_head = "<<write_channel.w_head<<" r_head = "<<write_channel.r_head<<" d_head = "<<write_channel.d_head);
    if(write_canreserve(n)){
      return (write_channel.w_head % buffer_size);
    }

    return 0;
  }

  //
  // Commit
  //
  // Read/Write shared memory data structure
  //
  void write_release(channel_t& ch){

    // insert your code here
    // write_channel.r_head = ch.r_head;  //to incorporate any changes that occured
    // write_channel.r_round = ch.r_round;
    // ch = write_channel;
    write_channel.w_head++;
    ch.w_pnt = write_channel.w_head;

  }


public:

  //
  //
  // Read/Write shared memory data structure
  //
  void read_acquire(channel_t& ch){

    //insert your code here
    // write_channel.r_head = ch.r_head;
    // write_channel.r_round = ch.r_round;
    write_channel.r_head = ch.r_pnt;
    hoh_debug("Reader is = "<<write_channel.r_head);
  }




  //
  // No of entires available to delete
  //
  size_t delete_reservesize(){
    //insert your code here
   if(write_channel.r_head-write_channel.d_head>=0 && buffer_size>=write_channel.r_head-write_channel.d_head){
      return (write_channel.r_head - write_channel.d_head);
    }
    return 0;
  }

  //
  // Can delete 'n' entires?
  //
  bool delete_canreserve(size_t n){
    //insert your code here
    if(delete_reservesize()>=n){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entires for deletion
  //
  size_t delete_reserve(size_t n){
    //insert your code here
    // hoh_debug("While deleting => w_head = "<<write_channel.w_head<<" r_head = "<<write_channel.r_head<<" d_head = "<<write_channel.d_head);
    if(delete_canreserve(n)){
      return (write_channel.d_head % buffer_size);
    }
    return 0;
  }


  //
  // Update the state, if any.
  //
  void delete_release(){
    //insert your code here
    write_channel.d_head++;
  }


};


//
// Consumer's (not shared)
//
//
struct readport_t{
public:

  //insert your code here
  channel_local read_channel;
  // int write_counter = -1;
  size_t buffer_size;


public:
  //
  // Initialize
  //
  readport_t(size_t tsize)
  {

    //insert your code here
    buffer_size = tsize;

  }
  public:

  //
  // Read/Write shared memory data structure
  //
  void write_acquire(channel_t& ch){

    //insert your code here
    // read_channel.w_head = ch.w_head;
    // read_channel.d_head = ch.d_head;

    // read_channel.w_round = ch.w_round;
    // read_channel.d_round = ch.d_round;
    read_channel.w_head = ch.w_pnt;
    hoh_debug("Writer is = "<<read_channel.w_head);
  }

  //
  // no of entries available to read
  //
  size_t read_reservesize(){

    //insert your code here
    if(read_channel.w_head - read_channel.r_head>=0 && buffer_size>= read_channel.w_head - read_channel.r_head){
      return (read_channel.w_head - read_channel.r_head);
    }
    else 
      return 0;
  }

  //
  // Can Read 'n' entires?
  //
  bool read_canreserve(size_t n){

    //insert your code here
    if(read_reservesize()>=n){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entires to be read
  //
  size_t read_reserve(size_t n){

    //insert your code here
    // hoh_debug("While reading => w_head = "<<read_channel.w_head<<" r_head = "<<read_channel.r_head);

    if(read_canreserve(n)){
      return (read_channel.r_head % buffer_size);
    }
    return 0;
  }

  //
  // Read/write shared memory data structure
  //
  void read_release(channel_t& ch){

    //insert your code here
    // read_channel.w_head = ch.w_head;
    // read_channel.d_head = ch.d_head;

    // read_channel.w_round = ch.w_round;
    // read_channel.d_round = ch.d_round;

    // ch = read_channel;
    read_channel.r_head++;
    ch.r_pnt = read_channel.r_head;

  }

};


