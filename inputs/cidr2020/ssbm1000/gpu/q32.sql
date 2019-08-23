{
  "operator" : "print",
  "gpu" : false,
  "e" : [ {
    "expression" : "recordProjection",
    "e" : {
      "expression" : "argument",
      "attributes" : [ {
        "attrName" : "c_city",
        "relName" : "subsetPelagoSort#5232"
      } ],
      "type" : {
        "relName" : "subsetPelagoSort#5232",
        "type" : "record"
      },
      "argNo" : -1
    },
    "attribute" : {
      "attrName" : "c_city",
      "relName" : "subsetPelagoSort#5232"
    },
    "register_as" : {
      "attrName" : "c_city",
      "relName" : "print5233"
    }
  }, {
    "expression" : "recordProjection",
    "e" : {
      "expression" : "argument",
      "attributes" : [ {
        "attrName" : "s_city",
        "relName" : "subsetPelagoSort#5232"
      } ],
      "type" : {
        "relName" : "subsetPelagoSort#5232",
        "type" : "record"
      },
      "argNo" : -1
    },
    "attribute" : {
      "attrName" : "s_city",
      "relName" : "subsetPelagoSort#5232"
    },
    "register_as" : {
      "attrName" : "s_city",
      "relName" : "print5233"
    }
  }, {
    "expression" : "recordProjection",
    "e" : {
      "expression" : "argument",
      "attributes" : [ {
        "attrName" : "d_year",
        "relName" : "subsetPelagoSort#5232"
      } ],
      "type" : {
        "relName" : "subsetPelagoSort#5232",
        "type" : "record"
      },
      "argNo" : -1
    },
    "attribute" : {
      "attrName" : "d_year",
      "relName" : "subsetPelagoSort#5232"
    },
    "register_as" : {
      "attrName" : "d_year",
      "relName" : "print5233"
    }
  }, {
    "expression" : "recordProjection",
    "e" : {
      "expression" : "argument",
      "attributes" : [ {
        "attrName" : "lo_revenue",
        "relName" : "subsetPelagoSort#5232"
      } ],
      "type" : {
        "relName" : "subsetPelagoSort#5232",
        "type" : "record"
      },
      "argNo" : -1
    },
    "attribute" : {
      "attrName" : "lo_revenue",
      "relName" : "subsetPelagoSort#5232"
    },
    "register_as" : {
      "attrName" : "lo_revenue",
      "relName" : "print5233"
    }
  } ],
  "input" : {
    "operator" : "project",
    "gpu" : false,
    "e" : [ {
      "e" : {
        "e" : {
          "attributes" : [ {
            "attrName" : "__sorted",
            "relName" : "__sort5232"
          } ],
          "expression" : "argument",
          "type" : {
            "type" : "record",
            "relName" : "__sort5232"
          },
          "argNo" : 1
        },
        "expression" : "recordProjection",
        "attribute" : {
          "attrName" : "__sorted",
          "relName" : "__sort5232"
        }
      },
      "expression" : "recordProjection",
      "attribute" : {
        "attrName" : "c_city",
        "relName" : "__sort5232"
      },
      "register_as" : {
        "attrName" : "c_city",
        "relName" : "subsetPelagoSort#5232"
      }
    }, {
      "e" : {
        "e" : {
          "attributes" : [ {
            "attrName" : "__sorted",
            "relName" : "__sort5232"
          } ],
          "expression" : "argument",
          "type" : {
            "type" : "record",
            "relName" : "__sort5232"
          },
          "argNo" : 1
        },
        "expression" : "recordProjection",
        "attribute" : {
          "attrName" : "__sorted",
          "relName" : "__sort5232"
        }
      },
      "expression" : "recordProjection",
      "attribute" : {
        "attrName" : "s_city",
        "relName" : "__sort5232"
      },
      "register_as" : {
        "attrName" : "s_city",
        "relName" : "subsetPelagoSort#5232"
      }
    }, {
      "e" : {
        "e" : {
          "attributes" : [ {
            "attrName" : "__sorted",
            "relName" : "__sort5232"
          } ],
          "expression" : "argument",
          "type" : {
            "type" : "record",
            "relName" : "__sort5232"
          },
          "argNo" : 1
        },
        "expression" : "recordProjection",
        "attribute" : {
          "attrName" : "__sorted",
          "relName" : "__sort5232"
        }
      },
      "expression" : "recordProjection",
      "attribute" : {
        "attrName" : "d_year",
        "relName" : "__sort5232"
      },
      "register_as" : {
        "attrName" : "d_year",
        "relName" : "subsetPelagoSort#5232"
      }
    }, {
      "e" : {
        "e" : {
          "attributes" : [ {
            "attrName" : "__sorted",
            "relName" : "__sort5232"
          } ],
          "expression" : "argument",
          "type" : {
            "type" : "record",
            "relName" : "__sort5232"
          },
          "argNo" : 1
        },
        "expression" : "recordProjection",
        "attribute" : {
          "attrName" : "__sorted",
          "relName" : "__sort5232"
        }
      },
      "expression" : "recordProjection",
      "attribute" : {
        "attrName" : "lo_revenue",
        "relName" : "__sort5232"
      },
      "register_as" : {
        "attrName" : "lo_revenue",
        "relName" : "subsetPelagoSort#5232"
      }
    } ],
    "relName" : "subsetPelagoSort#5232",
    "input" : {
      "operator" : "unpack",
      "gpu" : false,
      "projections" : [ {
        "e" : {
          "attributes" : [ {
            "attrName" : "__sorted",
            "relName" : "__sort5232"
          } ],
          "expression" : "argument",
          "type" : {
            "type" : "record",
            "relName" : "__sort5232"
          },
          "argNo" : 1
        },
        "expression" : "recordProjection",
        "attribute" : {
          "attrName" : "__sorted",
          "relName" : "__sort5232"
        }
      } ],
      "input" : {
        "operator" : "sort",
        "gpu" : false,
        "rowType" : [ {
          "relName" : "__sort5232",
          "attrName" : "c_city"
        }, {
          "relName" : "__sort5232",
          "attrName" : "s_city"
        }, {
          "relName" : "__sort5232",
          "attrName" : "d_year"
        }, {
          "relName" : "__sort5232",
          "attrName" : "lo_revenue"
        } ],
        "e" : [ {
          "direction" : "ASC",
          "expression" : {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "d_year",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "d_year",
              "relName" : "subsetPelagoUnpack#5231"
            },
            "register_as" : {
              "attrName" : "d_year",
              "relName" : "__sort5232"
            }
          }
        }, {
          "direction" : "DESC",
          "expression" : {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "lo_revenue",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "lo_revenue",
              "relName" : "subsetPelagoUnpack#5231"
            },
            "register_as" : {
              "attrName" : "lo_revenue",
              "relName" : "__sort5232"
            }
          }
        }, {
          "direction" : "NONE",
          "expression" : {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "c_city",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "c_city",
              "relName" : "subsetPelagoUnpack#5231"
            },
            "register_as" : {
              "attrName" : "c_city",
              "relName" : "__sort5232"
            }
          }
        }, {
          "direction" : "NONE",
          "expression" : {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "s_city",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "s_city",
              "relName" : "subsetPelagoUnpack#5231"
            },
            "register_as" : {
              "attrName" : "s_city",
              "relName" : "__sort5232"
            }
          }
        } ],
        "granularity" : "thread",
        "input" : {
          "operator" : "unpack",
          "gpu" : false,
          "projections" : [ {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "c_city",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "c_city",
              "relName" : "subsetPelagoUnpack#5231"
            }
          }, {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "s_city",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "s_city",
              "relName" : "subsetPelagoUnpack#5231"
            }
          }, {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "d_year",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "d_year",
              "relName" : "subsetPelagoUnpack#5231"
            }
          }, {
            "expression" : "recordProjection",
            "e" : {
              "expression" : "argument",
              "attributes" : [ {
                "attrName" : "lo_revenue",
                "relName" : "subsetPelagoUnpack#5231"
              } ],
              "type" : {
                "relName" : "subsetPelagoUnpack#5231",
                "type" : "record"
              },
              "argNo" : -1
            },
            "attribute" : {
              "attrName" : "lo_revenue",
              "relName" : "subsetPelagoUnpack#5231"
            }
          } ],
          "input" : {
            "operator" : "mem-move-device",
            "projections" : [ {
              "relName" : "subsetPelagoUnpack#5231",
              "attrName" : "c_city",
              "isBlock" : true
            }, {
              "relName" : "subsetPelagoUnpack#5231",
              "attrName" : "s_city",
              "isBlock" : true
            }, {
              "relName" : "subsetPelagoUnpack#5231",
              "attrName" : "d_year",
              "isBlock" : true
            }, {
              "relName" : "subsetPelagoUnpack#5231",
              "attrName" : "lo_revenue",
              "isBlock" : true
            } ],
            "input" : {
              "operator" : "gpu-to-cpu",
              "projections" : [ {
                "relName" : "subsetPelagoUnpack#5231",
                "attrName" : "c_city",
                "isBlock" : true
              }, {
                "relName" : "subsetPelagoUnpack#5231",
                "attrName" : "s_city",
                "isBlock" : true
              }, {
                "relName" : "subsetPelagoUnpack#5231",
                "attrName" : "d_year",
                "isBlock" : true
              }, {
                "relName" : "subsetPelagoUnpack#5231",
                "attrName" : "lo_revenue",
                "isBlock" : true
              } ],
              "queueSize" : 262144,
              "granularity" : "thread",
              "input" : {
                "operator" : "pack",
                "gpu" : true,
                "projections" : [ {
                  "expression" : "recordProjection",
                  "e" : {
                    "expression" : "argument",
                    "attributes" : [ {
                      "attrName" : "c_city",
                      "relName" : "subsetPelagoUnpack#5231"
                    } ],
                    "type" : {
                      "relName" : "subsetPelagoUnpack#5231",
                      "type" : "record"
                    },
                    "argNo" : -1
                  },
                  "attribute" : {
                    "attrName" : "c_city",
                    "relName" : "subsetPelagoUnpack#5231"
                  }
                }, {
                  "expression" : "recordProjection",
                  "e" : {
                    "expression" : "argument",
                    "attributes" : [ {
                      "attrName" : "s_city",
                      "relName" : "subsetPelagoUnpack#5231"
                    } ],
                    "type" : {
                      "relName" : "subsetPelagoUnpack#5231",
                      "type" : "record"
                    },
                    "argNo" : -1
                  },
                  "attribute" : {
                    "attrName" : "s_city",
                    "relName" : "subsetPelagoUnpack#5231"
                  }
                }, {
                  "expression" : "recordProjection",
                  "e" : {
                    "expression" : "argument",
                    "attributes" : [ {
                      "attrName" : "d_year",
                      "relName" : "subsetPelagoUnpack#5231"
                    } ],
                    "type" : {
                      "relName" : "subsetPelagoUnpack#5231",
                      "type" : "record"
                    },
                    "argNo" : -1
                  },
                  "attribute" : {
                    "attrName" : "d_year",
                    "relName" : "subsetPelagoUnpack#5231"
                  }
                }, {
                  "expression" : "recordProjection",
                  "e" : {
                    "expression" : "argument",
                    "attributes" : [ {
                      "attrName" : "lo_revenue",
                      "relName" : "subsetPelagoUnpack#5231"
                    } ],
                    "type" : {
                      "relName" : "subsetPelagoUnpack#5231",
                      "type" : "record"
                    },
                    "argNo" : -1
                  },
                  "attribute" : {
                    "attrName" : "lo_revenue",
                    "relName" : "subsetPelagoUnpack#5231"
                  }
                } ],
                "input" : {
                  "operator" : "groupby",
                  "gpu" : true,
                  "k" : [ {
                    "expression" : "recordProjection",
                    "e" : {
                      "expression" : "argument",
                      "attributes" : [ {
                        "attrName" : "c_city",
                        "relName" : "subsetPelagoUnpack#5227"
                      } ],
                      "type" : {
                        "relName" : "subsetPelagoUnpack#5227",
                        "type" : "record"
                      },
                      "argNo" : -1
                    },
                    "attribute" : {
                      "attrName" : "c_city",
                      "relName" : "subsetPelagoUnpack#5227"
                    },
                    "register_as" : {
                      "attrName" : "c_city",
                      "relName" : "subsetPelagoUnpack#5231"
                    }
                  }, {
                    "expression" : "recordProjection",
                    "e" : {
                      "expression" : "argument",
                      "attributes" : [ {
                        "attrName" : "s_city",
                        "relName" : "subsetPelagoUnpack#5227"
                      } ],
                      "type" : {
                        "relName" : "subsetPelagoUnpack#5227",
                        "type" : "record"
                      },
                      "argNo" : -1
                    },
                    "attribute" : {
                      "attrName" : "s_city",
                      "relName" : "subsetPelagoUnpack#5227"
                    },
                    "register_as" : {
                      "attrName" : "s_city",
                      "relName" : "subsetPelagoUnpack#5231"
                    }
                  }, {
                    "expression" : "recordProjection",
                    "e" : {
                      "expression" : "argument",
                      "attributes" : [ {
                        "attrName" : "d_year",
                        "relName" : "subsetPelagoUnpack#5227"
                      } ],
                      "type" : {
                        "relName" : "subsetPelagoUnpack#5227",
                        "type" : "record"
                      },
                      "argNo" : -1
                    },
                    "attribute" : {
                      "attrName" : "d_year",
                      "relName" : "subsetPelagoUnpack#5227"
                    },
                    "register_as" : {
                      "attrName" : "d_year",
                      "relName" : "subsetPelagoUnpack#5231"
                    }
                  } ],
                  "e" : [ {
                    "m" : "sum",
                    "e" : {
                      "expression" : "recordProjection",
                      "e" : {
                        "expression" : "argument",
                        "attributes" : [ {
                          "attrName" : "lo_revenue",
                          "relName" : "subsetPelagoUnpack#5227"
                        } ],
                        "type" : {
                          "relName" : "subsetPelagoUnpack#5227",
                          "type" : "record"
                        },
                        "argNo" : -1
                      },
                      "attribute" : {
                        "attrName" : "lo_revenue",
                        "relName" : "subsetPelagoUnpack#5227"
                      },
                      "register_as" : {
                        "attrName" : "lo_revenue",
                        "relName" : "subsetPelagoUnpack#5231"
                      }
                    },
                    "packet" : 1,
                    "offset" : 0
                  } ],
                  "hash_bits" : 10,
                  "maxInputSize" : 131072,
                  "input" : {
                    "operator" : "unpack",
                    "gpu" : true,
                    "projections" : [ {
                      "expression" : "recordProjection",
                      "e" : {
                        "expression" : "argument",
                        "attributes" : [ {
                          "attrName" : "c_city",
                          "relName" : "subsetPelagoUnpack#5227"
                        } ],
                        "type" : {
                          "relName" : "subsetPelagoUnpack#5227",
                          "type" : "record"
                        },
                        "argNo" : -1
                      },
                      "attribute" : {
                        "attrName" : "c_city",
                        "relName" : "subsetPelagoUnpack#5227"
                      }
                    }, {
                      "expression" : "recordProjection",
                      "e" : {
                        "expression" : "argument",
                        "attributes" : [ {
                          "attrName" : "s_city",
                          "relName" : "subsetPelagoUnpack#5227"
                        } ],
                        "type" : {
                          "relName" : "subsetPelagoUnpack#5227",
                          "type" : "record"
                        },
                        "argNo" : -1
                      },
                      "attribute" : {
                        "attrName" : "s_city",
                        "relName" : "subsetPelagoUnpack#5227"
                      }
                    }, {
                      "expression" : "recordProjection",
                      "e" : {
                        "expression" : "argument",
                        "attributes" : [ {
                          "attrName" : "d_year",
                          "relName" : "subsetPelagoUnpack#5227"
                        } ],
                        "type" : {
                          "relName" : "subsetPelagoUnpack#5227",
                          "type" : "record"
                        },
                        "argNo" : -1
                      },
                      "attribute" : {
                        "attrName" : "d_year",
                        "relName" : "subsetPelagoUnpack#5227"
                      }
                    }, {
                      "expression" : "recordProjection",
                      "e" : {
                        "expression" : "argument",
                        "attributes" : [ {
                          "attrName" : "lo_revenue",
                          "relName" : "subsetPelagoUnpack#5227"
                        } ],
                        "type" : {
                          "relName" : "subsetPelagoUnpack#5227",
                          "type" : "record"
                        },
                        "argNo" : -1
                      },
                      "attribute" : {
                        "attrName" : "lo_revenue",
                        "relName" : "subsetPelagoUnpack#5227"
                      }
                    } ],
                    "input" : {
                      "operator" : "cpu-to-gpu",
                      "projections" : [ {
                        "relName" : "subsetPelagoUnpack#5227",
                        "attrName" : "c_city",
                        "isBlock" : true
                      }, {
                        "relName" : "subsetPelagoUnpack#5227",
                        "attrName" : "s_city",
                        "isBlock" : true
                      }, {
                        "relName" : "subsetPelagoUnpack#5227",
                        "attrName" : "d_year",
                        "isBlock" : true
                      }, {
                        "relName" : "subsetPelagoUnpack#5227",
                        "attrName" : "lo_revenue",
                        "isBlock" : true
                      } ],
                      "queueSize" : 262144,
                      "granularity" : "thread",
                      "input" : {
                        "operator" : "mem-move-device",
                        "projections" : [ {
                          "relName" : "subsetPelagoUnpack#5227",
                          "attrName" : "c_city",
                          "isBlock" : true
                        }, {
                          "relName" : "subsetPelagoUnpack#5227",
                          "attrName" : "s_city",
                          "isBlock" : true
                        }, {
                          "relName" : "subsetPelagoUnpack#5227",
                          "attrName" : "d_year",
                          "isBlock" : true
                        }, {
                          "relName" : "subsetPelagoUnpack#5227",
                          "attrName" : "lo_revenue",
                          "isBlock" : true
                        } ],
                        "input" : {
                          "operator" : "router",
                          "gpu" : false,
                          "projections" : [ {
                            "relName" : "subsetPelagoUnpack#5227",
                            "attrName" : "c_city",
                            "isBlock" : true
                          }, {
                            "relName" : "subsetPelagoUnpack#5227",
                            "attrName" : "s_city",
                            "isBlock" : true
                          }, {
                            "relName" : "subsetPelagoUnpack#5227",
                            "attrName" : "d_year",
                            "isBlock" : true
                          }, {
                            "relName" : "subsetPelagoUnpack#5227",
                            "attrName" : "lo_revenue",
                            "isBlock" : true
                          } ],
                          "numOfParents" : 1,
                          "producers" : 2,
                          "slack" : 8,
                          "cpu_targets" : false,
                          "input" : {
                            "operator" : "mem-move-device",
                            "projections" : [ {
                              "relName" : "subsetPelagoUnpack#5227",
                              "attrName" : "c_city",
                              "isBlock" : true
                            }, {
                              "relName" : "subsetPelagoUnpack#5227",
                              "attrName" : "s_city",
                              "isBlock" : true
                            }, {
                              "relName" : "subsetPelagoUnpack#5227",
                              "attrName" : "d_year",
                              "isBlock" : true
                            }, {
                              "relName" : "subsetPelagoUnpack#5227",
                              "attrName" : "lo_revenue",
                              "isBlock" : true
                            } ],
                            "input" : {
                              "operator" : "gpu-to-cpu",
                              "projections" : [ {
                                "relName" : "subsetPelagoUnpack#5227",
                                "attrName" : "c_city",
                                "isBlock" : true
                              }, {
                                "relName" : "subsetPelagoUnpack#5227",
                                "attrName" : "s_city",
                                "isBlock" : true
                              }, {
                                "relName" : "subsetPelagoUnpack#5227",
                                "attrName" : "d_year",
                                "isBlock" : true
                              }, {
                                "relName" : "subsetPelagoUnpack#5227",
                                "attrName" : "lo_revenue",
                                "isBlock" : true
                              } ],
                              "queueSize" : 262144,
                              "granularity" : "thread",
                              "input" : {
                                "operator" : "pack",
                                "gpu" : true,
                                "projections" : [ {
                                  "expression" : "recordProjection",
                                  "e" : {
                                    "expression" : "argument",
                                    "attributes" : [ {
                                      "attrName" : "c_city",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    } ],
                                    "type" : {
                                      "relName" : "subsetPelagoUnpack#5227",
                                      "type" : "record"
                                    },
                                    "argNo" : -1
                                  },
                                  "attribute" : {
                                    "attrName" : "c_city",
                                    "relName" : "subsetPelagoUnpack#5227"
                                  }
                                }, {
                                  "expression" : "recordProjection",
                                  "e" : {
                                    "expression" : "argument",
                                    "attributes" : [ {
                                      "attrName" : "s_city",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    } ],
                                    "type" : {
                                      "relName" : "subsetPelagoUnpack#5227",
                                      "type" : "record"
                                    },
                                    "argNo" : -1
                                  },
                                  "attribute" : {
                                    "attrName" : "s_city",
                                    "relName" : "subsetPelagoUnpack#5227"
                                  }
                                }, {
                                  "expression" : "recordProjection",
                                  "e" : {
                                    "expression" : "argument",
                                    "attributes" : [ {
                                      "attrName" : "d_year",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    } ],
                                    "type" : {
                                      "relName" : "subsetPelagoUnpack#5227",
                                      "type" : "record"
                                    },
                                    "argNo" : -1
                                  },
                                  "attribute" : {
                                    "attrName" : "d_year",
                                    "relName" : "subsetPelagoUnpack#5227"
                                  }
                                }, {
                                  "expression" : "recordProjection",
                                  "e" : {
                                    "expression" : "argument",
                                    "attributes" : [ {
                                      "attrName" : "lo_revenue",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    } ],
                                    "type" : {
                                      "relName" : "subsetPelagoUnpack#5227",
                                      "type" : "record"
                                    },
                                    "argNo" : -1
                                  },
                                  "attribute" : {
                                    "attrName" : "lo_revenue",
                                    "relName" : "subsetPelagoUnpack#5227"
                                  }
                                } ],
                                "input" : {
                                  "operator" : "groupby",
                                  "gpu" : true,
                                  "k" : [ {
                                    "expression" : "recordProjection",
                                    "e" : {
                                      "expression" : "argument",
                                      "attributes" : [ {
                                        "attrName" : "c_city",
                                        "relName" : "subsetPelagoProject#5221"
                                      } ],
                                      "type" : {
                                        "relName" : "subsetPelagoProject#5221",
                                        "type" : "record"
                                      },
                                      "argNo" : -1
                                    },
                                    "attribute" : {
                                      "attrName" : "c_city",
                                      "relName" : "subsetPelagoProject#5221"
                                    },
                                    "register_as" : {
                                      "attrName" : "c_city",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    }
                                  }, {
                                    "expression" : "recordProjection",
                                    "e" : {
                                      "expression" : "argument",
                                      "attributes" : [ {
                                        "attrName" : "s_city",
                                        "relName" : "subsetPelagoProject#5221"
                                      } ],
                                      "type" : {
                                        "relName" : "subsetPelagoProject#5221",
                                        "type" : "record"
                                      },
                                      "argNo" : -1
                                    },
                                    "attribute" : {
                                      "attrName" : "s_city",
                                      "relName" : "subsetPelagoProject#5221"
                                    },
                                    "register_as" : {
                                      "attrName" : "s_city",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    }
                                  }, {
                                    "expression" : "recordProjection",
                                    "e" : {
                                      "expression" : "argument",
                                      "attributes" : [ {
                                        "attrName" : "d_year",
                                        "relName" : "subsetPelagoProject#5221"
                                      } ],
                                      "type" : {
                                        "relName" : "subsetPelagoProject#5221",
                                        "type" : "record"
                                      },
                                      "argNo" : -1
                                    },
                                    "attribute" : {
                                      "attrName" : "d_year",
                                      "relName" : "subsetPelagoProject#5221"
                                    },
                                    "register_as" : {
                                      "attrName" : "d_year",
                                      "relName" : "subsetPelagoUnpack#5227"
                                    }
                                  } ],
                                  "e" : [ {
                                    "m" : "sum",
                                    "e" : {
                                      "expression" : "recordProjection",
                                      "e" : {
                                        "expression" : "argument",
                                        "attributes" : [ {
                                          "attrName" : "lo_revenue",
                                          "relName" : "subsetPelagoProject#5221"
                                        } ],
                                        "type" : {
                                          "relName" : "subsetPelagoProject#5221",
                                          "type" : "record"
                                        },
                                        "argNo" : -1
                                      },
                                      "attribute" : {
                                        "attrName" : "lo_revenue",
                                        "relName" : "subsetPelagoProject#5221"
                                      },
                                      "register_as" : {
                                        "attrName" : "lo_revenue",
                                        "relName" : "subsetPelagoUnpack#5227"
                                      }
                                    },
                                    "packet" : 1,
                                    "offset" : 0
                                  } ],
                                  "hash_bits" : 10,
                                  "maxInputSize" : 131072,
                                  "input" : {
                                    "operator" : "project",
                                    "gpu" : true,
                                    "relName" : "subsetPelagoProject#5221",
                                    "e" : [ {
                                      "expression" : "recordProjection",
                                      "e" : {
                                        "expression" : "argument",
                                        "attributes" : [ {
                                          "attrName" : "c_city",
                                          "relName" : "subsetPelagoProject#5221"
                                        } ],
                                        "type" : {
                                          "relName" : "subsetPelagoProject#5221",
                                          "type" : "record"
                                        },
                                        "argNo" : -1
                                      },
                                      "attribute" : {
                                        "attrName" : "c_city",
                                        "relName" : "subsetPelagoProject#5221"
                                      },
                                      "register_as" : {
                                        "attrName" : "c_city",
                                        "relName" : "subsetPelagoProject#5221"
                                      }
                                    }, {
                                      "expression" : "recordProjection",
                                      "e" : {
                                        "expression" : "argument",
                                        "attributes" : [ {
                                          "attrName" : "s_city",
                                          "relName" : "subsetPelagoProject#5221"
                                        } ],
                                        "type" : {
                                          "relName" : "subsetPelagoProject#5221",
                                          "type" : "record"
                                        },
                                        "argNo" : -1
                                      },
                                      "attribute" : {
                                        "attrName" : "s_city",
                                        "relName" : "subsetPelagoProject#5221"
                                      },
                                      "register_as" : {
                                        "attrName" : "s_city",
                                        "relName" : "subsetPelagoProject#5221"
                                      }
                                    }, {
                                      "expression" : "recordProjection",
                                      "e" : {
                                        "expression" : "argument",
                                        "attributes" : [ {
                                          "attrName" : "d_year",
                                          "relName" : "subsetPelagoProject#5221"
                                        } ],
                                        "type" : {
                                          "relName" : "subsetPelagoProject#5221",
                                          "type" : "record"
                                        },
                                        "argNo" : -1
                                      },
                                      "attribute" : {
                                        "attrName" : "d_year",
                                        "relName" : "subsetPelagoProject#5221"
                                      },
                                      "register_as" : {
                                        "attrName" : "d_year",
                                        "relName" : "subsetPelagoProject#5221"
                                      }
                                    }, {
                                      "expression" : "recordProjection",
                                      "e" : {
                                        "expression" : "argument",
                                        "attributes" : [ {
                                          "attrName" : "lo_revenue",
                                          "relName" : "subsetPelagoProject#5221"
                                        } ],
                                        "type" : {
                                          "relName" : "subsetPelagoProject#5221",
                                          "type" : "record"
                                        },
                                        "argNo" : -1
                                      },
                                      "attribute" : {
                                        "attrName" : "lo_revenue",
                                        "relName" : "subsetPelagoProject#5221"
                                      },
                                      "register_as" : {
                                        "attrName" : "lo_revenue",
                                        "relName" : "subsetPelagoProject#5221"
                                      }
                                    } ],
                                    "input" : {
                                      "operator" : "hashjoin-chained",
                                      "gpu" : true,
                                      "build_k" : {
                                        "expression" : "recordProjection",
                                        "e" : {
                                          "expression" : "argument",
                                          "attributes" : [ {
                                            "attrName" : "c_custkey",
                                            "relName" : "subsetPelagoProject#5203"
                                          } ],
                                          "type" : {
                                            "relName" : "subsetPelagoProject#5203",
                                            "type" : "record"
                                          },
                                          "argNo" : -1
                                        },
                                        "attribute" : {
                                          "attrName" : "c_custkey",
                                          "relName" : "subsetPelagoProject#5203"
                                        },
                                        "register_as" : {
                                          "attrName" : "$2",
                                          "relName" : "subsetPelagoProject#5221"
                                        }
                                      },
                                      "build_e" : [ {
                                        "e" : {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "c_custkey",
                                              "relName" : "subsetPelagoProject#5203"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5203",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "c_custkey",
                                            "relName" : "subsetPelagoProject#5203"
                                          },
                                          "register_as" : {
                                            "attrName" : "c_custkey",
                                            "relName" : "subsetPelagoProject#5221"
                                          }
                                        },
                                        "packet" : 1,
                                        "offset" : 0
                                      }, {
                                        "e" : {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "c_city",
                                              "relName" : "subsetPelagoProject#5203"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5203",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "c_city",
                                            "relName" : "subsetPelagoProject#5203"
                                          },
                                          "register_as" : {
                                            "attrName" : "c_city",
                                            "relName" : "subsetPelagoProject#5221"
                                          }
                                        },
                                        "packet" : 2,
                                        "offset" : 0
                                      } ],
                                      "build_w" : [ 64, 32, 32 ],
                                      "build_input" : {
                                        "operator" : "project",
                                        "gpu" : true,
                                        "relName" : "subsetPelagoProject#5203",
                                        "e" : [ {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "c_custkey",
                                              "relName" : "inputs/ssbm1000/customer.csv"
                                            } ],
                                            "type" : {
                                              "relName" : "inputs/ssbm1000/customer.csv",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "c_custkey",
                                            "relName" : "inputs/ssbm1000/customer.csv"
                                          },
                                          "register_as" : {
                                            "attrName" : "c_custkey",
                                            "relName" : "subsetPelagoProject#5203"
                                          }
                                        }, {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "c_city",
                                              "relName" : "inputs/ssbm1000/customer.csv"
                                            } ],
                                            "type" : {
                                              "relName" : "inputs/ssbm1000/customer.csv",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "c_city",
                                            "relName" : "inputs/ssbm1000/customer.csv"
                                          },
                                          "register_as" : {
                                            "attrName" : "c_city",
                                            "relName" : "subsetPelagoProject#5203"
                                          }
                                        } ],
                                        "input" : {
                                          "operator" : "select",
                                          "gpu" : true,
                                          "p" : {
                                            "expression" : "eq",
                                            "left" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "c_nation",
                                                  "relName" : "inputs/ssbm1000/customer.csv"
                                                } ],
                                                "type" : {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "c_nation",
                                                "relName" : "inputs/ssbm1000/customer.csv"
                                              }
                                            },
                                            "right" : {
                                              "expression" : "dstring",
                                              "v" : "UNITED STATES",
                                              "dict" : {
                                                "path" : "inputs/ssbm1000/customer.csv.ProjectedRelDataTypeField(#2: c_nation VARCHAR,null).dict"
                                              }
                                            }
                                          },
                                          "input" : {
                                            "operator" : "unpack",
                                            "gpu" : true,
                                            "projections" : [ {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "c_custkey",
                                                  "relName" : "inputs/ssbm1000/customer.csv"
                                                } ],
                                                "type" : {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "c_custkey",
                                                "relName" : "inputs/ssbm1000/customer.csv"
                                              }
                                            }, {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "c_city",
                                                  "relName" : "inputs/ssbm1000/customer.csv"
                                                } ],
                                                "type" : {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "c_city",
                                                "relName" : "inputs/ssbm1000/customer.csv"
                                              }
                                            }, {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "c_nation",
                                                  "relName" : "inputs/ssbm1000/customer.csv"
                                                } ],
                                                "type" : {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "c_nation",
                                                "relName" : "inputs/ssbm1000/customer.csv"
                                              }
                                            } ],
                                            "input" : {
                                              "operator" : "cpu-to-gpu",
                                              "projections" : [ {
                                                "relName" : "inputs/ssbm1000/customer.csv",
                                                "attrName" : "c_custkey",
                                                "isBlock" : true
                                              }, {
                                                "relName" : "inputs/ssbm1000/customer.csv",
                                                "attrName" : "c_city",
                                                "isBlock" : true
                                              }, {
                                                "relName" : "inputs/ssbm1000/customer.csv",
                                                "attrName" : "c_nation",
                                                "isBlock" : true
                                              } ],
                                              "queueSize" : 262144,
                                              "granularity" : "thread",
                                              "input" : {
                                                "operator" : "router",
                                                "gpu" : false,
                                                "projections" : [ {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "attrName" : "c_custkey",
                                                  "isBlock" : true
                                                }, {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "attrName" : "c_city",
                                                  "isBlock" : true
                                                }, {
                                                  "relName" : "inputs/ssbm1000/customer.csv",
                                                  "attrName" : "c_nation",
                                                  "isBlock" : true
                                                } ],
                                                "numOfParents" : 2,
                                                "producers" : 1,
                                                "slack" : 8,
                                                "cpu_targets" : false,
                                                "target" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "argNo" : -1,
                                                    "type" : {
                                                      "type" : "record",
                                                      "relName" : "inputs/ssbm1000/customer.csv"
                                                    },
                                                    "attributes" : [ {
                                                      "relName" : "inputs/ssbm1000/customer.csv",
                                                      "attrName" : "__broadcastTarget"
                                                    } ]
                                                  },
                                                  "attribute" : {
                                                    "relName" : "inputs/ssbm1000/customer.csv",
                                                    "attrName" : "__broadcastTarget"
                                                  }
                                                },
                                                "input" : {
                                                  "operator" : "mem-broadcast-device",
                                                  "num_of_targets" : 2,
                                                  "projections" : [ {
                                                    "relName" : "inputs/ssbm1000/customer.csv",
                                                    "attrName" : "c_custkey",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/customer.csv",
                                                    "attrName" : "c_city",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/customer.csv",
                                                    "attrName" : "c_nation",
                                                    "isBlock" : true
                                                  } ],
                                                  "input" : {
                                                    "operator" : "scan",
                                                    "gpu" : false,
                                                    "plugin" : {
                                                      "type" : "block",
                                                      "linehint" : 30000000,
                                                      "name" : "inputs/ssbm1000/customer.csv",
                                                      "projections" : [ {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_custkey"
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_city"
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_nation"
                                                      } ],
                                                      "schema" : [ {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_custkey",
                                                        "type" : {
                                                          "type" : "int"
                                                        },
                                                        "attrNo" : 1
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_name",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 2
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_address",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 3
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_city",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 4
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_nation",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 5
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_region",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 6
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_phone",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 7
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/customer.csv",
                                                        "attrName" : "c_mktsegment",
                                                        "type" : {
                                                          "type" : "dstring"
                                                        },
                                                        "attrNo" : 8
                                                      } ]
                                                    }
                                                  },
                                                  "to_cpu" : false
                                                }
                                              }
                                            }
                                          }
                                        }
                                      },
                                      "probe_k" : {
                                        "expression" : "recordProjection",
                                        "e" : {
                                          "expression" : "argument",
                                          "attributes" : [ {
                                            "attrName" : "lo_custkey",
                                            "relName" : "subsetPelagoProject#5219"
                                          } ],
                                          "type" : {
                                            "relName" : "subsetPelagoProject#5219",
                                            "type" : "record"
                                          },
                                          "argNo" : -1
                                        },
                                        "attribute" : {
                                          "attrName" : "lo_custkey",
                                          "relName" : "subsetPelagoProject#5219"
                                        },
                                        "register_as" : {
                                          "attrName" : "$0",
                                          "relName" : "subsetPelagoProject#5221"
                                        }
                                      },
                                      "probe_e" : [ {
                                        "e" : {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "lo_custkey",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "lo_custkey",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "lo_custkey",
                                            "relName" : "subsetPelagoProject#5221"
                                          }
                                        },
                                        "packet" : 1,
                                        "offset" : 0
                                      }, {
                                        "e" : {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "lo_revenue",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "lo_revenue",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "lo_revenue",
                                            "relName" : "subsetPelagoProject#5221"
                                          }
                                        },
                                        "packet" : 2,
                                        "offset" : 0
                                      }, {
                                        "e" : {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "d_year",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "d_year",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "d_year",
                                            "relName" : "subsetPelagoProject#5221"
                                          }
                                        },
                                        "packet" : 3,
                                        "offset" : 0
                                      }, {
                                        "e" : {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "s_city",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "s_city",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "s_city",
                                            "relName" : "subsetPelagoProject#5221"
                                          }
                                        },
                                        "packet" : 4,
                                        "offset" : 0
                                      } ],
                                      "probe_w" : [ 64, 32, 32, 32, 32 ],
                                      "hash_bits" : 12,
                                      "maxBuildInputSize" : 30000000,
                                      "probe_input" : {
                                        "operator" : "project",
                                        "gpu" : true,
                                        "relName" : "subsetPelagoProject#5219",
                                        "e" : [ {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "lo_custkey",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "lo_custkey",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "lo_custkey",
                                            "relName" : "subsetPelagoProject#5219"
                                          }
                                        }, {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "lo_revenue",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "lo_revenue",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "lo_revenue",
                                            "relName" : "subsetPelagoProject#5219"
                                          }
                                        }, {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "d_year",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "d_year",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "d_year",
                                            "relName" : "subsetPelagoProject#5219"
                                          }
                                        }, {
                                          "expression" : "recordProjection",
                                          "e" : {
                                            "expression" : "argument",
                                            "attributes" : [ {
                                              "attrName" : "s_city",
                                              "relName" : "subsetPelagoProject#5219"
                                            } ],
                                            "type" : {
                                              "relName" : "subsetPelagoProject#5219",
                                              "type" : "record"
                                            },
                                            "argNo" : -1
                                          },
                                          "attribute" : {
                                            "attrName" : "s_city",
                                            "relName" : "subsetPelagoProject#5219"
                                          },
                                          "register_as" : {
                                            "attrName" : "s_city",
                                            "relName" : "subsetPelagoProject#5219"
                                          }
                                        } ],
                                        "input" : {
                                          "operator" : "hashjoin-chained",
                                          "gpu" : true,
                                          "build_k" : {
                                            "expression" : "recordProjection",
                                            "e" : {
                                              "expression" : "argument",
                                              "attributes" : [ {
                                                "attrName" : "s_suppkey",
                                                "relName" : "subsetPelagoProject#5208"
                                              } ],
                                              "type" : {
                                                "relName" : "subsetPelagoProject#5208",
                                                "type" : "record"
                                              },
                                              "argNo" : -1
                                            },
                                            "attribute" : {
                                              "attrName" : "s_suppkey",
                                              "relName" : "subsetPelagoProject#5208"
                                            },
                                            "register_as" : {
                                              "attrName" : "$3",
                                              "relName" : "subsetPelagoProject#5219"
                                            }
                                          },
                                          "build_e" : [ {
                                            "e" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "s_suppkey",
                                                  "relName" : "subsetPelagoProject#5208"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5208",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "s_suppkey",
                                                "relName" : "subsetPelagoProject#5208"
                                              },
                                              "register_as" : {
                                                "attrName" : "s_suppkey",
                                                "relName" : "subsetPelagoProject#5219"
                                              }
                                            },
                                            "packet" : 1,
                                            "offset" : 0
                                          }, {
                                            "e" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "s_city",
                                                  "relName" : "subsetPelagoProject#5208"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5208",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "s_city",
                                                "relName" : "subsetPelagoProject#5208"
                                              },
                                              "register_as" : {
                                                "attrName" : "s_city",
                                                "relName" : "subsetPelagoProject#5219"
                                              }
                                            },
                                            "packet" : 2,
                                            "offset" : 0
                                          } ],
                                          "build_w" : [ 64, 32, 32 ],
                                          "build_input" : {
                                            "operator" : "project",
                                            "gpu" : true,
                                            "relName" : "subsetPelagoProject#5208",
                                            "e" : [ {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "s_suppkey",
                                                  "relName" : "inputs/ssbm1000/supplier.csv"
                                                } ],
                                                "type" : {
                                                  "relName" : "inputs/ssbm1000/supplier.csv",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "s_suppkey",
                                                "relName" : "inputs/ssbm1000/supplier.csv"
                                              },
                                              "register_as" : {
                                                "attrName" : "s_suppkey",
                                                "relName" : "subsetPelagoProject#5208"
                                              }
                                            }, {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "s_city",
                                                  "relName" : "inputs/ssbm1000/supplier.csv"
                                                } ],
                                                "type" : {
                                                  "relName" : "inputs/ssbm1000/supplier.csv",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "s_city",
                                                "relName" : "inputs/ssbm1000/supplier.csv"
                                              },
                                              "register_as" : {
                                                "attrName" : "s_city",
                                                "relName" : "subsetPelagoProject#5208"
                                              }
                                            } ],
                                            "input" : {
                                              "operator" : "select",
                                              "gpu" : true,
                                              "p" : {
                                                "expression" : "eq",
                                                "left" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "s_nation",
                                                      "relName" : "inputs/ssbm1000/supplier.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "s_nation",
                                                    "relName" : "inputs/ssbm1000/supplier.csv"
                                                  }
                                                },
                                                "right" : {
                                                  "expression" : "dstring",
                                                  "v" : "UNITED STATES",
                                                  "dict" : {
                                                    "path" : "inputs/ssbm1000/supplier.csv.ProjectedRelDataTypeField(#2: s_nation VARCHAR,null).dict"
                                                  }
                                                }
                                              },
                                              "input" : {
                                                "operator" : "unpack",
                                                "gpu" : true,
                                                "projections" : [ {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "s_suppkey",
                                                      "relName" : "inputs/ssbm1000/supplier.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "s_suppkey",
                                                    "relName" : "inputs/ssbm1000/supplier.csv"
                                                  }
                                                }, {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "s_city",
                                                      "relName" : "inputs/ssbm1000/supplier.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "s_city",
                                                    "relName" : "inputs/ssbm1000/supplier.csv"
                                                  }
                                                }, {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "s_nation",
                                                      "relName" : "inputs/ssbm1000/supplier.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "s_nation",
                                                    "relName" : "inputs/ssbm1000/supplier.csv"
                                                  }
                                                } ],
                                                "input" : {
                                                  "operator" : "cpu-to-gpu",
                                                  "projections" : [ {
                                                    "relName" : "inputs/ssbm1000/supplier.csv",
                                                    "attrName" : "s_suppkey",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/supplier.csv",
                                                    "attrName" : "s_city",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/supplier.csv",
                                                    "attrName" : "s_nation",
                                                    "isBlock" : true
                                                  } ],
                                                  "queueSize" : 262144,
                                                  "granularity" : "thread",
                                                  "input" : {
                                                    "operator" : "router",
                                                    "gpu" : false,
                                                    "projections" : [ {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "attrName" : "s_suppkey",
                                                      "isBlock" : true
                                                    }, {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "attrName" : "s_city",
                                                      "isBlock" : true
                                                    }, {
                                                      "relName" : "inputs/ssbm1000/supplier.csv",
                                                      "attrName" : "s_nation",
                                                      "isBlock" : true
                                                    } ],
                                                    "numOfParents" : 2,
                                                    "producers" : 1,
                                                    "slack" : 8,
                                                    "cpu_targets" : false,
                                                    "target" : {
                                                      "expression" : "recordProjection",
                                                      "e" : {
                                                        "expression" : "argument",
                                                        "argNo" : -1,
                                                        "type" : {
                                                          "type" : "record",
                                                          "relName" : "inputs/ssbm1000/supplier.csv"
                                                        },
                                                        "attributes" : [ {
                                                          "relName" : "inputs/ssbm1000/supplier.csv",
                                                          "attrName" : "__broadcastTarget"
                                                        } ]
                                                      },
                                                      "attribute" : {
                                                        "relName" : "inputs/ssbm1000/supplier.csv",
                                                        "attrName" : "__broadcastTarget"
                                                      }
                                                    },
                                                    "input" : {
                                                      "operator" : "mem-broadcast-device",
                                                      "num_of_targets" : 2,
                                                      "projections" : [ {
                                                        "relName" : "inputs/ssbm1000/supplier.csv",
                                                        "attrName" : "s_suppkey",
                                                        "isBlock" : true
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/supplier.csv",
                                                        "attrName" : "s_city",
                                                        "isBlock" : true
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/supplier.csv",
                                                        "attrName" : "s_nation",
                                                        "isBlock" : true
                                                      } ],
                                                      "input" : {
                                                        "operator" : "scan",
                                                        "gpu" : false,
                                                        "plugin" : {
                                                          "type" : "block",
                                                          "linehint" : 2000000,
                                                          "name" : "inputs/ssbm1000/supplier.csv",
                                                          "projections" : [ {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_suppkey"
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_city"
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_nation"
                                                          } ],
                                                          "schema" : [ {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_suppkey",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 1
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_name",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 2
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_address",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 3
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_city",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 4
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_nation",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 5
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_region",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 6
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/supplier.csv",
                                                            "attrName" : "s_phone",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 7
                                                          } ]
                                                        }
                                                      },
                                                      "to_cpu" : false
                                                    }
                                                  }
                                                }
                                              }
                                            }
                                          },
                                          "probe_k" : {
                                            "expression" : "recordProjection",
                                            "e" : {
                                              "expression" : "argument",
                                              "attributes" : [ {
                                                "attrName" : "lo_suppkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              } ],
                                              "type" : {
                                                "relName" : "subsetPelagoProject#5217",
                                                "type" : "record"
                                              },
                                              "argNo" : -1
                                            },
                                            "attribute" : {
                                              "attrName" : "lo_suppkey",
                                              "relName" : "subsetPelagoProject#5217"
                                            },
                                            "register_as" : {
                                              "attrName" : "$0",
                                              "relName" : "subsetPelagoProject#5219"
                                            }
                                          },
                                          "probe_e" : [ {
                                            "e" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "lo_custkey",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "lo_custkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "lo_custkey",
                                                "relName" : "subsetPelagoProject#5219"
                                              }
                                            },
                                            "packet" : 1,
                                            "offset" : 0
                                          }, {
                                            "e" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "lo_suppkey",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "lo_suppkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "lo_suppkey",
                                                "relName" : "subsetPelagoProject#5219"
                                              }
                                            },
                                            "packet" : 2,
                                            "offset" : 0
                                          }, {
                                            "e" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "lo_revenue",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "lo_revenue",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "lo_revenue",
                                                "relName" : "subsetPelagoProject#5219"
                                              }
                                            },
                                            "packet" : 3,
                                            "offset" : 0
                                          }, {
                                            "e" : {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "d_year",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "d_year",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "d_year",
                                                "relName" : "subsetPelagoProject#5219"
                                              }
                                            },
                                            "packet" : 4,
                                            "offset" : 0
                                          } ],
                                          "probe_w" : [ 64, 32, 32, 32, 32 ],
                                          "hash_bits" : 12,
                                          "maxBuildInputSize" : 2000000,
                                          "probe_input" : {
                                            "operator" : "project",
                                            "gpu" : true,
                                            "relName" : "subsetPelagoProject#5217",
                                            "e" : [ {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "lo_custkey",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "lo_custkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "lo_custkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              }
                                            }, {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "lo_suppkey",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "lo_suppkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "lo_suppkey",
                                                "relName" : "subsetPelagoProject#5217"
                                              }
                                            }, {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "lo_revenue",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "lo_revenue",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "lo_revenue",
                                                "relName" : "subsetPelagoProject#5217"
                                              }
                                            }, {
                                              "expression" : "recordProjection",
                                              "e" : {
                                                "expression" : "argument",
                                                "attributes" : [ {
                                                  "attrName" : "d_year",
                                                  "relName" : "subsetPelagoProject#5217"
                                                } ],
                                                "type" : {
                                                  "relName" : "subsetPelagoProject#5217",
                                                  "type" : "record"
                                                },
                                                "argNo" : -1
                                              },
                                              "attribute" : {
                                                "attrName" : "d_year",
                                                "relName" : "subsetPelagoProject#5217"
                                              },
                                              "register_as" : {
                                                "attrName" : "d_year",
                                                "relName" : "subsetPelagoProject#5217"
                                              }
                                            } ],
                                            "input" : {
                                              "operator" : "hashjoin-chained",
                                              "gpu" : true,
                                              "build_k" : {
                                                "expression" : "recordProjection",
                                                "e" : {
                                                  "expression" : "argument",
                                                  "attributes" : [ {
                                                    "attrName" : "d_datekey",
                                                    "relName" : "inputs/ssbm1000/date.csv"
                                                  } ],
                                                  "type" : {
                                                    "relName" : "inputs/ssbm1000/date.csv",
                                                    "type" : "record"
                                                  },
                                                  "argNo" : -1
                                                },
                                                "attribute" : {
                                                  "attrName" : "d_datekey",
                                                  "relName" : "inputs/ssbm1000/date.csv"
                                                },
                                                "register_as" : {
                                                  "attrName" : "$4",
                                                  "relName" : "subsetPelagoProject#5217"
                                                }
                                              },
                                              "build_e" : [ {
                                                "e" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "d_datekey",
                                                      "relName" : "inputs/ssbm1000/date.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/date.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "d_datekey",
                                                    "relName" : "inputs/ssbm1000/date.csv"
                                                  },
                                                  "register_as" : {
                                                    "attrName" : "d_datekey",
                                                    "relName" : "subsetPelagoProject#5217"
                                                  }
                                                },
                                                "packet" : 1,
                                                "offset" : 0
                                              }, {
                                                "e" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "d_year",
                                                      "relName" : "inputs/ssbm1000/date.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/date.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "d_year",
                                                    "relName" : "inputs/ssbm1000/date.csv"
                                                  },
                                                  "register_as" : {
                                                    "attrName" : "d_year",
                                                    "relName" : "subsetPelagoProject#5217"
                                                  }
                                                },
                                                "packet" : 2,
                                                "offset" : 0
                                              } ],
                                              "build_w" : [ 64, 32, 32 ],
                                              "build_input" : {
                                                "operator" : "select",
                                                "gpu" : true,
                                                "p" : {
                                                  "expression" : "and",
                                                  "left" : {
                                                    "expression" : "ge",
                                                    "left" : {
                                                      "expression" : "recordProjection",
                                                      "e" : {
                                                        "expression" : "argument",
                                                        "attributes" : [ {
                                                          "attrName" : "d_year",
                                                          "relName" : "inputs/ssbm1000/date.csv"
                                                        } ],
                                                        "type" : {
                                                          "relName" : "inputs/ssbm1000/date.csv",
                                                          "type" : "record"
                                                        },
                                                        "argNo" : -1
                                                      },
                                                      "attribute" : {
                                                        "attrName" : "d_year",
                                                        "relName" : "inputs/ssbm1000/date.csv"
                                                      }
                                                    },
                                                    "right" : {
                                                      "expression" : "int",
                                                      "v" : 1992
                                                    }
                                                  },
                                                  "right" : {
                                                    "expression" : "le",
                                                    "left" : {
                                                      "expression" : "recordProjection",
                                                      "e" : {
                                                        "expression" : "argument",
                                                        "attributes" : [ {
                                                          "attrName" : "d_year",
                                                          "relName" : "inputs/ssbm1000/date.csv"
                                                        } ],
                                                        "type" : {
                                                          "relName" : "inputs/ssbm1000/date.csv",
                                                          "type" : "record"
                                                        },
                                                        "argNo" : -1
                                                      },
                                                      "attribute" : {
                                                        "attrName" : "d_year",
                                                        "relName" : "inputs/ssbm1000/date.csv"
                                                      }
                                                    },
                                                    "right" : {
                                                      "expression" : "int",
                                                      "v" : 1997
                                                    }
                                                  }
                                                },
                                                "input" : {
                                                  "operator" : "unpack",
                                                  "gpu" : true,
                                                  "projections" : [ {
                                                    "expression" : "recordProjection",
                                                    "e" : {
                                                      "expression" : "argument",
                                                      "attributes" : [ {
                                                        "attrName" : "d_datekey",
                                                        "relName" : "inputs/ssbm1000/date.csv"
                                                      } ],
                                                      "type" : {
                                                        "relName" : "inputs/ssbm1000/date.csv",
                                                        "type" : "record"
                                                      },
                                                      "argNo" : -1
                                                    },
                                                    "attribute" : {
                                                      "attrName" : "d_datekey",
                                                      "relName" : "inputs/ssbm1000/date.csv"
                                                    }
                                                  }, {
                                                    "expression" : "recordProjection",
                                                    "e" : {
                                                      "expression" : "argument",
                                                      "attributes" : [ {
                                                        "attrName" : "d_year",
                                                        "relName" : "inputs/ssbm1000/date.csv"
                                                      } ],
                                                      "type" : {
                                                        "relName" : "inputs/ssbm1000/date.csv",
                                                        "type" : "record"
                                                      },
                                                      "argNo" : -1
                                                    },
                                                    "attribute" : {
                                                      "attrName" : "d_year",
                                                      "relName" : "inputs/ssbm1000/date.csv"
                                                    }
                                                  } ],
                                                  "input" : {
                                                    "operator" : "cpu-to-gpu",
                                                    "projections" : [ {
                                                      "relName" : "inputs/ssbm1000/date.csv",
                                                      "attrName" : "d_datekey",
                                                      "isBlock" : true
                                                    }, {
                                                      "relName" : "inputs/ssbm1000/date.csv",
                                                      "attrName" : "d_year",
                                                      "isBlock" : true
                                                    } ],
                                                    "queueSize" : 262144,
                                                    "granularity" : "thread",
                                                    "input" : {
                                                      "operator" : "router",
                                                      "gpu" : false,
                                                      "projections" : [ {
                                                        "relName" : "inputs/ssbm1000/date.csv",
                                                        "attrName" : "d_datekey",
                                                        "isBlock" : true
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/date.csv",
                                                        "attrName" : "d_year",
                                                        "isBlock" : true
                                                      } ],
                                                      "numOfParents" : 2,
                                                      "producers" : 1,
                                                      "slack" : 8,
                                                      "cpu_targets" : false,
                                                      "target" : {
                                                        "expression" : "recordProjection",
                                                        "e" : {
                                                          "expression" : "argument",
                                                          "argNo" : -1,
                                                          "type" : {
                                                            "type" : "record",
                                                            "relName" : "inputs/ssbm1000/date.csv"
                                                          },
                                                          "attributes" : [ {
                                                            "relName" : "inputs/ssbm1000/date.csv",
                                                            "attrName" : "__broadcastTarget"
                                                          } ]
                                                        },
                                                        "attribute" : {
                                                          "relName" : "inputs/ssbm1000/date.csv",
                                                          "attrName" : "__broadcastTarget"
                                                        }
                                                      },
                                                      "input" : {
                                                        "operator" : "mem-broadcast-device",
                                                        "num_of_targets" : 2,
                                                        "projections" : [ {
                                                          "relName" : "inputs/ssbm1000/date.csv",
                                                          "attrName" : "d_datekey",
                                                          "isBlock" : true
                                                        }, {
                                                          "relName" : "inputs/ssbm1000/date.csv",
                                                          "attrName" : "d_year",
                                                          "isBlock" : true
                                                        } ],
                                                        "input" : {
                                                          "operator" : "scan",
                                                          "gpu" : false,
                                                          "plugin" : {
                                                            "type" : "block",
                                                            "linehint" : 2556,
                                                            "name" : "inputs/ssbm1000/date.csv",
                                                            "projections" : [ {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_datekey"
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_year"
                                                            } ],
                                                            "schema" : [ {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_datekey",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 1
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_date",
                                                              "type" : {
                                                                "type" : "dstring"
                                                              },
                                                              "attrNo" : 2
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_dayofweek",
                                                              "type" : {
                                                                "type" : "dstring"
                                                              },
                                                              "attrNo" : 3
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_month",
                                                              "type" : {
                                                                "type" : "dstring"
                                                              },
                                                              "attrNo" : 4
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_year",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 5
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_yearmonthnum",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 6
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_yearmonth",
                                                              "type" : {
                                                                "type" : "dstring"
                                                              },
                                                              "attrNo" : 7
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_daynuminweek",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 8
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_daynuminmonth",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 9
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_daynuminyear",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 10
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_monthnuminyear",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 11
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_weeknuminyear",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 12
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_sellingseason",
                                                              "type" : {
                                                                "type" : "dstring"
                                                              },
                                                              "attrNo" : 13
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_lastdayinweekfl",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 14
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_lastdayinmonthfl",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 15
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_holidayfl",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 16
                                                            }, {
                                                              "relName" : "inputs/ssbm1000/date.csv",
                                                              "attrName" : "d_weekdayfl",
                                                              "type" : {
                                                                "type" : "int"
                                                              },
                                                              "attrNo" : 17
                                                            } ]
                                                          }
                                                        },
                                                        "to_cpu" : false
                                                      }
                                                    }
                                                  }
                                                }
                                              },
                                              "probe_k" : {
                                                "expression" : "recordProjection",
                                                "e" : {
                                                  "expression" : "argument",
                                                  "attributes" : [ {
                                                    "attrName" : "lo_orderdate",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  } ],
                                                  "type" : {
                                                    "relName" : "inputs/ssbm1000/lineorder.csv",
                                                    "type" : "record"
                                                  },
                                                  "argNo" : -1
                                                },
                                                "attribute" : {
                                                  "attrName" : "lo_orderdate",
                                                  "relName" : "inputs/ssbm1000/lineorder.csv"
                                                },
                                                "register_as" : {
                                                  "attrName" : "$0",
                                                  "relName" : "subsetPelagoProject#5217"
                                                }
                                              },
                                              "probe_e" : [ {
                                                "e" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_custkey",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_custkey",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  },
                                                  "register_as" : {
                                                    "attrName" : "lo_custkey",
                                                    "relName" : "subsetPelagoProject#5217"
                                                  }
                                                },
                                                "packet" : 1,
                                                "offset" : 0
                                              }, {
                                                "e" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_suppkey",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_suppkey",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  },
                                                  "register_as" : {
                                                    "attrName" : "lo_suppkey",
                                                    "relName" : "subsetPelagoProject#5217"
                                                  }
                                                },
                                                "packet" : 2,
                                                "offset" : 0
                                              }, {
                                                "e" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_orderdate",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_orderdate",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  },
                                                  "register_as" : {
                                                    "attrName" : "lo_orderdate",
                                                    "relName" : "subsetPelagoProject#5217"
                                                  }
                                                },
                                                "packet" : 3,
                                                "offset" : 0
                                              }, {
                                                "e" : {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_revenue",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_revenue",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  },
                                                  "register_as" : {
                                                    "attrName" : "lo_revenue",
                                                    "relName" : "subsetPelagoProject#5217"
                                                  }
                                                },
                                                "packet" : 4,
                                                "offset" : 0
                                              } ],
                                              "probe_w" : [ 64, 32, 32, 32, 32 ],
                                              "hash_bits" : 2,
                                              "maxBuildInputSize" : 2556,
                                              "probe_input" : {
                                                "operator" : "unpack",
                                                "gpu" : true,
                                                "projections" : [ {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_custkey",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_custkey",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  }
                                                }, {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_suppkey",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_suppkey",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  }
                                                }, {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_orderdate",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_orderdate",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  }
                                                }, {
                                                  "expression" : "recordProjection",
                                                  "e" : {
                                                    "expression" : "argument",
                                                    "attributes" : [ {
                                                      "attrName" : "lo_revenue",
                                                      "relName" : "inputs/ssbm1000/lineorder.csv"
                                                    } ],
                                                    "type" : {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "type" : "record"
                                                    },
                                                    "argNo" : -1
                                                  },
                                                  "attribute" : {
                                                    "attrName" : "lo_revenue",
                                                    "relName" : "inputs/ssbm1000/lineorder.csv"
                                                  }
                                                } ],
                                                "input" : {
                                                  "operator" : "cpu-to-gpu",
                                                  "projections" : [ {
                                                    "relName" : "inputs/ssbm1000/lineorder.csv",
                                                    "attrName" : "lo_custkey",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/lineorder.csv",
                                                    "attrName" : "lo_suppkey",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/lineorder.csv",
                                                    "attrName" : "lo_orderdate",
                                                    "isBlock" : true
                                                  }, {
                                                    "relName" : "inputs/ssbm1000/lineorder.csv",
                                                    "attrName" : "lo_revenue",
                                                    "isBlock" : true
                                                  } ],
                                                  "queueSize" : 262144,
                                                  "granularity" : "thread",
                                                  "input" : {
                                                    "operator" : "mem-move-device",
                                                    "projections" : [ {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "attrName" : "lo_custkey",
                                                      "isBlock" : true
                                                    }, {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "attrName" : "lo_suppkey",
                                                      "isBlock" : true
                                                    }, {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "attrName" : "lo_orderdate",
                                                      "isBlock" : true
                                                    }, {
                                                      "relName" : "inputs/ssbm1000/lineorder.csv",
                                                      "attrName" : "lo_revenue",
                                                      "isBlock" : true
                                                    } ],
                                                    "input" : {
                                                      "operator" : "router",
                                                      "gpu" : false,
                                                      "projections" : [ {
                                                        "relName" : "inputs/ssbm1000/lineorder.csv",
                                                        "attrName" : "lo_custkey",
                                                        "isBlock" : true
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/lineorder.csv",
                                                        "attrName" : "lo_suppkey",
                                                        "isBlock" : true
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/lineorder.csv",
                                                        "attrName" : "lo_orderdate",
                                                        "isBlock" : true
                                                      }, {
                                                        "relName" : "inputs/ssbm1000/lineorder.csv",
                                                        "attrName" : "lo_revenue",
                                                        "isBlock" : true
                                                      } ],
                                                      "numOfParents" : 2,
                                                      "producers" : 1,
                                                      "slack" : 8,
                                                      "cpu_targets" : false,
                                                      "numa_local" : true,
                                                      "input" : {
                                                        "operator" : "scan",
                                                        "gpu" : false,
                                                        "plugin" : {
                                                          "type" : "block",
                                                          "linehint" : 5999989813,
                                                          "name" : "inputs/ssbm1000/lineorder.csv",
                                                          "projections" : [ {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_custkey"
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_suppkey"
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_orderdate"
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_revenue"
                                                          } ],
                                                          "schema" : [ {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_orderkey",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 1
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_linenumber",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 2
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_custkey",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 3
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_partkey",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 4
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_suppkey",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 5
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_orderdate",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 6
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_orderpriority",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 7
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_shippriority",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 8
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_quantity",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 9
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_extendedprice",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 10
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_ordtotalprice",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 11
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_discount",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 12
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_revenue",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 13
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_supplycost",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 14
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_tax",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 15
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_commitdate",
                                                            "type" : {
                                                              "type" : "int"
                                                            },
                                                            "attrNo" : 16
                                                          }, {
                                                            "relName" : "inputs/ssbm1000/lineorder.csv",
                                                            "attrName" : "lo_shipmode",
                                                            "type" : {
                                                              "type" : "dstring"
                                                            },
                                                            "attrNo" : 17
                                                          } ]
                                                        }
                                                      }
                                                    },
                                                    "to_cpu" : false
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                },
                                "trait" : "Pelago.[].packed.NVPTX.homRandom.hetSingle"
                              }
                            },
                            "to_cpu" : false
                          }
                        },
                        "to_cpu" : false
                      }
                    }
                  }
                },
                "trait" : "Pelago.[].packed.NVPTX.homSingle.hetSingle"
              }
            },
            "to_cpu" : true
          }
        }
      }
    }
  }
}